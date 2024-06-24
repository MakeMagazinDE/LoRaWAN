#include "main.h"
#include "app.h"
#include "base.h"

void app_init() {
  int a = 42;
  int* ptr_a = &a;

  while (1) {
    APP_LOG(TS_OFF, VLEVEL_L, "Adresse von a: %x\r\n", (void*)ptr_a);
    APP_LOG(TS_OFF, VLEVEL_L, "Inhalt von a:  %d\r\n", *ptr_a);
    HAL_Delay(10 * 1000);
  }
}
