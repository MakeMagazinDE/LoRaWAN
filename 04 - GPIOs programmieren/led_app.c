#include "main.h"
#include "app.h"
#include "base.h"
#include "hw_gpio.h"
#include "led.h"
#include "sys_app.h"

void app_init() {
  GPIO_InitTypeDef pinConfig = { 0 };
  pinConfig.Mode  = GPIO_MODE_OUTPUT_PP;
  pinConfig.Pull  = GPIO_NOPULL;
  pinConfig.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  HW_GPIO_Init(GPIOB, GPIO_PIN_14, &pinConfig);
  
  while (1) {
    APP_LOG(TS_OFF, VLEVEL_L, "LED on.\r\n");
    HW_GPIO_Write(GPIOB, GPIO_PIN_14, 1);
    HAL_Delay(1000);

    APP_LOG(TS_OFF, VLEVEL_L, "LED off.\r\n");
    HW_GPIO_Write(GPIOB, GPIO_PIN_14, 0);
    HAL_Delay(1000);
  }
}
