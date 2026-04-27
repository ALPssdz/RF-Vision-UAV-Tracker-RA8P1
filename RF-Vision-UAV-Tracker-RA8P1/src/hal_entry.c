#include <rtthread.h>

#include "app_config.h"
#include "app_types.h"
#include "rf_detector_simple.h"
#include "status_led.h"
#include "telemetry.h"

#define DBG_TAG     "edge"
#define DBG_LVL     DBG_LOG
#include <rtdbg.h>

static const rt_uint32_t g_sector_freq_mhz[APP_SECTOR_COUNT] = {5745U, 5785U, 5825U};

static void mock_sdr_read(app_rf_sample_t * sample, rt_uint32_t tick, rt_uint32_t sector_index)
{
    rt_uint32_t cycle = tick % 32U;

    sample->center_freq_mhz = g_sector_freq_mhz[sector_index];
    sample->noise_floor_q10 = 1024U;
    sample->rssi_q10 = 980U + sector_index * 24U;
    sample->kurtosis_q8 = 3U * APP_KURTOSIS_SCALE;

    if ((sector_index == 1U) && (cycle >= 8U) && (cycle <= 18U))
    {
        sample->rssi_q10 = 1880U;
        sample->kurtosis_q8 = 6U * APP_KURTOSIS_SCALE;
    }
}

static void detector_thread_entry(void * parameter)
{
    rt_uint32_t tick = 0;
    rt_uint32_t sector = 0;
    rt_bool_t last_alert[APP_SECTOR_COUNT] = {RT_FALSE};

    RT_UNUSED(parameter);

    rf_detector_init();
    status_led_set_scan();

    while (1)
    {
        app_rf_sample_t sample;
        app_detection_event_t event;
        rt_bool_t detected;

        mock_sdr_read(&sample, tick, sector);
        detected = rf_detector_process(&sample, &event);

        if (detected)
        {
            status_led_set_alert();
            if (!last_alert[sector])
            {
                telemetry_send_alert(&event);
            }
        }
        else
        {
            status_led_set_scan();
            if ((tick % 8U) == 0U)
            {
                telemetry_send_status(&event);
            }
        }

        last_alert[sector] = detected;
        sector = (sector + 1U) % APP_SECTOR_COUNT;
        tick++;
        rt_thread_mdelay(APP_DETECT_PERIOD_MS);
    }
}

static void heartbeat_thread_entry(void * parameter)
{
    RT_UNUSED(parameter);

    while (1)
    {
        status_led_tick();
        rt_thread_mdelay(APP_HEARTBEAT_PERIOD_MS);
    }
}

void hal_entry(void)
{
    rt_thread_t detector_thread;
    rt_thread_t heartbeat_thread;

    rt_kprintf("\nRF-Vision RA8P1 edge terminal\n");
    rt_kprintf("Topology: SDR -> RA8P1 -> center host\n");
    rt_kprintf("Mode: lightweight RF pre-detection, no K230, no YOLO GUI\n");

    status_led_init();
    telemetry_init();
    telemetry_send_boot();

    detector_thread = rt_thread_create("rf_detect",
                                       detector_thread_entry,
                                       RT_NULL,
                                       4096,
                                       12,
                                       10);
    if (detector_thread != RT_NULL)
    {
        rt_thread_startup(detector_thread);
    }
    else
    {
        LOG_E("failed to create rf_detect thread");
    }

    heartbeat_thread = rt_thread_create("heartbeat",
                                        heartbeat_thread_entry,
                                        RT_NULL,
                                        1024,
                                        20,
                                        10);
    if (heartbeat_thread != RT_NULL)
    {
        rt_thread_startup(heartbeat_thread);
    }
    else
    {
        LOG_E("failed to create heartbeat thread");
    }
}

