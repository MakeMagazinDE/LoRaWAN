#include "main.h"
#include "app.h"
#include "base.h"

typedef void (*Callback)(int, int);

void foo(int a, int b) { 
  APP_LOG(TS_OFF, VLEVEL_L, "a = %d, b = %d\r\n", a, b); 
} 

void bar(Callback callback) {
  callback(42, 15);
}

void app_init() {
  Callback foo_ptr = &foo;

  while (1) {
    bar(foo_ptr);
    HAL_Delay(10 * 1000);
  }
} 
