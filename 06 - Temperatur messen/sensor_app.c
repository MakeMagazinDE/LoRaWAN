#include "main.h"
#include "app.h"
#include "base.h"
#include "hw_gpio.h"
#include "sys_app.h"
#include "adc_if.h"
#include "hdc2080.h"
#include "103AT_2B_Values.h"

const uint32_t ADC_CHANNEL_NTC_TH1 = ADC_CHANNEL_10;

typedef struct th1_sensor_data {
  int16_t ntc_temperature;
  int16_t hdc2080_temperature;
  uint8_t hdc2080_humidity;
} th1_sensor_data_t;

static bool is_hdc2080_init = false;
static bool ELV_AM_TH1_present = false;

void read_sensor_data(th1_sensor_data_t*);

void app_init() {
  hdc2080_init();
  is_hdc2080_init = true;
  if (hdc2080_validate_manufacturer_device_id()) {
    ELV_AM_TH1_present = true;
    hdc2080_conf_single();
  }

  while (1) {
    if (ELV_AM_TH1_present) {
      th1_sensor_data_t sensor_data = { 0 };
      read_sensor_data(&sensor_data);

      switch ((uint16_t)sensor_data.ntc_temperature) {
      case TEMPERATURE_UNKNOWN:
      case TEMPERATURE_OVERFLOW:
      case TEMPERATURE_UNDERFLOW:
        APP_LOG(TS_OFF, VLEVEL_L, "Externe Temperatur: Fehler\r\n");
        break;
      default:
        APP_LOG(TS_OFF, VLEVEL_L, "Externe Temperatur:  %3u.%1u C\r\n", (sensor_data.ntc_temperature / 10), (sensor_data.ntc_temperature % 10));
        break;
      }

      APP_LOG(TS_OFF, VLEVEL_L, "HDC2080-Temperatur:  %3u.%1u C\r\n", (sensor_data.hdc2080_temperature / 10), (sensor_data.hdc2080_temperature % 10));
      APP_LOG(TS_OFF, VLEVEL_L, "HDC2080-Luftfeuchte: %5u %%\r\n", sensor_data.hdc2080_humidity);
    }

    HAL_Delay(500);
  }
}

int16_t get_ntc_temperature() {
  base_init_en_adc_supply();
  base_en_adc_supply(true);
  HAL_Delay(10);
  const uint16_t ntc_voltage_mv = adc_get_channel_level(ADC_CHANNEL_NTC_TH1);
  base_deinit_en_adc_supply();
  const uint16_t supply_voltage_mv = base_get_supply_level();
  return NTC_103AT_2B_VALUES_get_temperature_2(supply_voltage_mv, ntc_voltage_mv);
}

void read_sensor_data(th1_sensor_data_t* sensor_data) {
  sensor_data->ntc_temperature = get_ntc_temperature();

  hdc2080_start_conversion();
  hdc2080_wait_for_data(&is_hdc2080_init);
  const uint32_t hdc2080_data = hdc2080_get_temperature_humidity();
  const double hdc2080_temperature = (hdc2080_decode_temperature((uint16_t)(hdc2080_data >> 16))) * 10.0;

  if (hdc2080_temperature < 0) {
    sensor_data->hdc2080_temperature = (int16_t)(hdc2080_temperature - 0.5);
  } else {
    sensor_data->hdc2080_temperature = (int16_t)(hdc2080_temperature + 0.5);
  }
  sensor_data->hdc2080_humidity = (uint8_t)(hdc2080_decode_humidity((uint16_t)hdc2080_data) + 0.5);
}
