#include "main.h"
#include "app.h"
#include "base.h"
#include "hw_gpio.h"
#include "led.h"
#include "sys_app.h"

static base_callbacks_t base_app_cb = {
  .base_user_button_event = app_user_button_event,
};

static bool user_button_pressed = false;

void app_init() {
  base_init(&base_app_cb);
  leds_init();

  while (1) {
    if (app_get_user_button_pressed()) {
      APP_LOG( TS_OFF, VLEVEL_L, "User-Button gedrückt.\r\n" );
      led_red_toggle();
    }

    HAL_Delay(100);
  }
}

void app_user_button_event() {
  app_set_user_button_pressed(true);
}

void app_set_user_button_pressed(bool button_pressed) {
  user_button_pressed = button_pressed;
}

bool app_get_user_button_pressed() {
  bool state = user_button_pressed;

  if (state) {
    app_set_user_button_pressed(false);
    base_enable_irqs();
  }

  return state;
}
