#include "status_led.h"

#include <rtdevice.h>
#include <board.h>

#define LED_PIN_R   BSP_IO_PORT_00_PIN_13
#define LED_PIN_B   BSP_IO_PORT_00_PIN_12
#define LED_PIN_G   BSP_IO_PORT_06_PIN_13

#define LED_ON      0
#define LED_OFF     1

typedef enum
{
    LED_STATE_IDLE = 0,
    LED_STATE_SCAN,
    LED_STATE_ALERT,
} led_state_t;

static led_state_t g_state = LED_STATE_IDLE;
static rt_bool_t g_phase = RT_FALSE;

static void led_write(rt_uint8_t r, rt_uint8_t g, rt_uint8_t b)
{
    rt_pin_write(LED_PIN_R, r);
    rt_pin_write(LED_PIN_G, g);
    rt_pin_write(LED_PIN_B, b);
}

void status_led_init(void)
{
    rt_pin_mode(LED_PIN_R, PIN_MODE_OUTPUT);
    rt_pin_mode(LED_PIN_G, PIN_MODE_OUTPUT);
    rt_pin_mode(LED_PIN_B, PIN_MODE_OUTPUT);
    led_write(LED_OFF, LED_OFF, LED_OFF);
}

void status_led_set_idle(void)
{
    g_state = LED_STATE_IDLE;
}

void status_led_set_scan(void)
{
    g_state = LED_STATE_SCAN;
}

void status_led_set_alert(void)
{
    g_state = LED_STATE_ALERT;
}

void status_led_tick(void)
{
    g_phase = !g_phase;

    switch (g_state)
    {
    case LED_STATE_IDLE:
        led_write(LED_OFF, LED_OFF, g_phase ? LED_ON : LED_OFF);
        break;
    case LED_STATE_SCAN:
        led_write(LED_OFF, g_phase ? LED_ON : LED_OFF, LED_OFF);
        break;
    case LED_STATE_ALERT:
        led_write(g_phase ? LED_ON : LED_OFF, LED_OFF, LED_OFF);
        break;
    default:
        led_write(LED_OFF, LED_OFF, LED_OFF);
        break;
    }
}
