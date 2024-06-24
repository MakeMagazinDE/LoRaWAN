#include "main.h"
#include "app.h"
#include "base.h"

void foo(int a, int b) { 
  APP_LOG(TS_OFF, VLEVEL_L, "a = %d, b = %d\r\n", a, b); 
} 

void app_init() {
  void (*function_ptr)(int, int) = &foo;

  while (1) {
    (*function_ptr)(42, 15);
    HAL_Delay(10 * 1000);
  }
}
