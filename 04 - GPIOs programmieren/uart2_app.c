#include "main.h"
#include "app.h"
#include "base.h"
#include "hw_gpio.h"
#include "led.h"
#include "sys_app.h"
#include "usart.h"

void app_init() {
  MX_USART2_UART_Init();
  leds_init();

  uint8_t led_on[] = "LED on.\r\n";
  uint8_t led_off[] = "LED off.\r\n";
  uint8_t u8_rx_buffer[] = "?";

  while (1) {
    if (HAL_UART_Receive(&huart2, u8_rx_buffer, 1, 100) != HAL_TIMEOUT) {
      if (u8_rx_buffer[0] == '1') {
        HAL_UART_Transmit(&huart2, led_on, sizeof(led_on) - 1, 1000);
        led_red_en(true);
      } else if (u8_rx_buffer[0] == '0') {
          HAL_UART_Transmit(&huart2, led_off, sizeof(led_off) - 1, 1000);
          led_red_en(false);
      }
    }
  }
}