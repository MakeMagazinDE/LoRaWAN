#include "main.h"
#include "app.h"
#include "base.h"
#include "hw_gpio.h"
#include "led.h"
#include "sys_app.h"

void app_init() {
  GPIO_InitTypeDef initStruct = { 0 };
  initStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  initStruct.Pull  = GPIO_NOPULL;
  initStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HW_GPIO_Init(GPIOA, GPIO_PIN_5, &initStruct );
  
  while (1) {
    HW_GPIO_Write(GPIOA, GPIO_PIN_5, 1);
    HAL_Delay(1000);
    for (uint16_t i = 0; i < 500; i++) {
      HW_GPIO_Write(GPIOA, GPIO_PIN_5, 1);
      HAL_Delay(1);
      HW_GPIO_Write(GPIOA, GPIO_PIN_5, 0);
      HAL_Delay(1);
    }
    HW_GPIO_Write(GPIOA, GPIO_PIN_5, 0);
    HAL_Delay(1000);
  }
}
