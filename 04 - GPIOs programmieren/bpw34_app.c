#include "main.h"
#include "app.h"
#include "base.h"
#include "adc_if.h"
#include "hw_gpio.h"
#include "led.h"
#include "sys_app.h"

void app_init() {
  while (1) {
    const uint16_t voltage = adc_get_channel_level(ADC_CHANNEL_8);
    APP_LOG(TS_OFF, VLEVEL_L, "Spannung: %u mV\r\n", voltage);
    HAL_Delay(100);
  }
}
