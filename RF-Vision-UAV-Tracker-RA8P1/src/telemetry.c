#include "telemetry.h"

#include "app_config.h"
#include "telemetry_transport.h"

#define TELEMETRY_BUF_SIZE 256U

static rt_uint32_t g_telemetry_seq;

static rt_uint32_t telemetry_next_seq(void)
{
    g_telemetry_seq++;
    if (g_telemetry_seq == 0U)
    {
        g_telemetry_seq = 1U;
    }
    return g_telemetry_seq;
}

static rt_uint32_t telemetry_uptime_ms(void)
{
    rt_tick_t tick = rt_tick_get();
    return (rt_uint32_t)(((rt_uint64_t)tick * 1000ULL) / RT_TICK_PER_SECOND);
}

static void telemetry_send_raw(const char * payload)
{
    (void)telemetry_transport_send(payload);
}

void telemetry_init(void)
{
    g_telemetry_seq = 0U;
    (void)telemetry_transport_init();
    rt_kprintf("[telemetry] target=%s:%u\n",
               APP_TELEMETRY_CENTER_HOST,
               APP_TELEMETRY_CENTER_PORT);
}

void telemetry_send_boot(void)
{
    char buf[TELEMETRY_BUF_SIZE];

    rt_snprintf(buf, sizeof(buf),
                "{\"type\":\"boot\",\"device\":\"%s\",\"seq\":%u,"
                "\"uptime_ms\":%u,\"platform\":\"RA8P1\",\"os\":\"RT-Thread\"}",
                APP_DEVICE_ID,
                telemetry_next_seq(),
                telemetry_uptime_ms());
    telemetry_send_raw(buf);
}

void telemetry_send_status(const app_detection_event_t * event)
{
    char buf[TELEMETRY_BUF_SIZE];

    if (event == RT_NULL)
    {
        return;
    }

    rt_snprintf(buf, sizeof(buf),
                "{\"type\":\"status\",\"device\":\"%s\",\"seq\":%u,"
                "\"uptime_ms\":%u,\"freq_mhz\":%u,\"score_q10\":%u,"
                "\"confidence\":%u,\"confirm\":%u}",
                APP_DEVICE_ID,
                telemetry_next_seq(),
                telemetry_uptime_ms(),
                event->center_freq_mhz,
                event->score_q10,
                event->confidence,
                event->confirm_count);
    telemetry_send_raw(buf);
}

void telemetry_send_alert(const app_detection_event_t * event)
{
    char buf[TELEMETRY_BUF_SIZE];

    if (event == RT_NULL)
    {
        return;
    }

    rt_snprintf(buf, sizeof(buf),
                "{\"type\":\"uav_alert\",\"device\":\"%s\",\"seq\":%u,"
                "\"uptime_ms\":%u,\"freq_mhz\":%u,\"score_q10\":%u,"
                "\"confidence\":%u,\"confirm\":%u}",
                APP_DEVICE_ID,
                telemetry_next_seq(),
                telemetry_uptime_ms(),
                event->center_freq_mhz,
                event->score_q10,
                event->confidence,
                event->confirm_count);
    telemetry_send_raw(buf);
}
