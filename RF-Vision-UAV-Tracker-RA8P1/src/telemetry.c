#include "telemetry.h"

#include "app_config.h"

void telemetry_init(void)
{
    rt_kprintf("[telemetry] sink=console target=%s:%u\n",
               APP_TELEMETRY_CENTER_HOST,
               APP_TELEMETRY_CENTER_PORT);
}

void telemetry_send_boot(void)
{
    rt_kprintf("{\"type\":\"boot\",\"device\":\"%s\",\"platform\":\"RA8P1\",\"os\":\"RT-Thread\"}\n",
               APP_DEVICE_ID);
}

void telemetry_send_status(const app_detection_event_t * event)
{
    if (event == RT_NULL)
    {
        return;
    }

    rt_kprintf("{\"type\":\"status\",\"device\":\"%s\",\"freq_mhz\":%u,"
               "\"score_q10\":%u,\"confidence\":%u,\"confirm\":%u}\n",
               APP_DEVICE_ID,
               event->center_freq_mhz,
               event->score_q10,
               event->confidence,
               event->confirm_count);
}

void telemetry_send_alert(const app_detection_event_t * event)
{
    if (event == RT_NULL)
    {
        return;
    }

    rt_kprintf("{\"type\":\"uav_alert\",\"device\":\"%s\",\"freq_mhz\":%u,"
               "\"score_q10\":%u,\"confidence\":%u,\"confirm\":%u}\n",
               APP_DEVICE_ID,
               event->center_freq_mhz,
               event->score_q10,
               event->confidence,
               event->confirm_count);
}
