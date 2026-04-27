#include <rtthread.h>

#include "app_config.h"
#include "app_types.h"
#include "telemetry.h"

#ifdef FINSH_USING_MSH
#include <finsh.h>

static void rf_info(int argc, char **argv)
{
    RT_UNUSED(argc);
    RT_UNUSED(argv);

    rt_kprintf("RF-Vision RA8P1 terminal\n");
    rt_kprintf("  device_id      : %s\n", APP_DEVICE_ID);
    rt_kprintf("  sectors        : %u (5745/5785/5825 MHz)\n", APP_SECTOR_COUNT);
    rt_kprintf("  detect_period  : %u ms\n", APP_DETECT_PERIOD_MS);
    rt_kprintf("  score_threshold: %u q10\n", APP_SCORE_THRESHOLD_Q10);
    rt_kprintf("  confirm_count  : %u\n", APP_CONFIRM_REQUIRED);
    rt_kprintf("  cooldown_ticks : %u\n", APP_ALERT_COOLDOWN_TICKS);
    rt_kprintf("  center         : %s:%u\n", APP_TELEMETRY_CENTER_HOST, APP_TELEMETRY_CENTER_PORT);
#if APP_TELEMETRY_TRANSPORT == APP_TELEMETRY_TRANSPORT_UDP
    rt_kprintf("  transport      : udp\n");
#else
    rt_kprintf("  transport      : console\n");
#endif
}
MSH_CMD_EXPORT(rf_info, show RF-Vision terminal configuration);

static void rf_send_test(int argc, char **argv)
{
    app_detection_event_t event;

    RT_UNUSED(argc);
    RT_UNUSED(argv);

    event.detected = RT_TRUE;
    event.center_freq_mhz = 5785U;
    event.score_q10 = 2648U;
    event.confidence = 86U;
    event.confirm_count = APP_CONFIRM_REQUIRED;

    telemetry_send_alert(&event);
}
MSH_CMD_EXPORT(rf_send_test, send a test UAV alert telemetry packet);

static void rf_send_status(int argc, char **argv)
{
    app_detection_event_t event;

    RT_UNUSED(argc);
    RT_UNUSED(argv);

    event.detected = RT_FALSE;
    event.center_freq_mhz = 5745U;
    event.score_q10 = 980U;
    event.confidence = 31U;
    event.confirm_count = 0U;

    telemetry_send_status(&event);
}
MSH_CMD_EXPORT(rf_send_status, send a test RF status telemetry packet);

#endif
