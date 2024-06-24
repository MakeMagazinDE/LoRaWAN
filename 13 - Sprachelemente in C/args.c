#include "main.h"
#include "app.h"
#include "base.h"

void foo(int x) {
  x = 42;
}

void bar(int* x) {
  *x = 42;
}

void app_init() {
  while (1) {
    int x = 5;

    foo(x);
    APP_LOG(TS_OFF, VLEVEL_L, "x = %d\r\n", x); // Ausgabe: x = 5
    bar(&x);
    APP_LOG(TS_OFF, VLEVEL_L, "x = %d\r\n", x); // Ausgabe: x = 42
    HAL_Delay(10 * 1000);
  }
}
