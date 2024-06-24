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

  HW_GPIO_Init(GPIOA, GPIO_PIN_5, &pinConfig);
  
  while (1) {
    HW_GPIO_Write(GPIOA, GPIO_PIN_5, 1);
    HAL_Delay(500);
    HW_GPIO_Write(GPIOA, GPIO_PIN_5, 0);
    HAL_Delay(500);
  }
}
