#include "main.h"
#include "app.h"
#include "base.h"
#include "hw_gpio.h"
#include "led.h"
#include "sys_app.h"

void app_init() {
  base_print_bl_and_app_version(
    (uint8_t*)APP_APPLICATION_NAME_STR,
    (uint8_t*)APP_APPLICATION_VERSION_STR);
  base_power_module_detection();

  leds_init();
  
  while (1) {
    APP_LOG(TS_OFF, VLEVEL_L, "Supply: %u mV\r\n", base_get_supply_level());
    led_red_toggle();
    led_green_toggle();
    HAL_Delay(500);
  }
}
