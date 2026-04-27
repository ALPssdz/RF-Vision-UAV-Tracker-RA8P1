#ifndef STATUS_LED_H
#define STATUS_LED_H

#include <rtthread.h>

void status_led_init(void);
void status_led_set_idle(void);
void status_led_set_scan(void);
void status_led_set_alert(void);
void status_led_tick(void);

#endif
