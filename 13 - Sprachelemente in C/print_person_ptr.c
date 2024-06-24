#include "main.h"
#include "app.h"
#include "base.h"

struct Person {
  unsigned char age;
  char name[20];
};

void print_person(struct Person* p) {
  APP_LOG(TS_OFF, VLEVEL_L, "%s: %d\r\n", p->name, p->age);
}

void app_init() {
  struct Person hans = { 42, "Hans" };

  while (1) {
    print_person(&hans);
    HAL_Delay(10 * 1000);
  }
}
