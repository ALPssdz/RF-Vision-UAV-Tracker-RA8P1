#include "sdr_frontend.h"

#include "app_config.h"

static const rt_uint32_t g_sector_freq_mhz[APP_SECTOR_COUNT] = {5745U, 5785U, 5825U};
static rt_uint32_t g_mock_tick;

rt_err_t sdr_frontend_init(void)
{
    g_mock_tick = 0;
    rt_kprintf("[sdr] frontend=mock sectors=%u\n", APP_SECTOR_COUNT);
    return RT_EOK;
}

rt_err_t sdr_frontend_read(rt_uint32_t sector_index, app_rf_sample_t * sample)
{
    rt_uint32_t cycle;

    if ((sample == RT_NULL) || (sector_index >= APP_SECTOR_COUNT))
    {
        return -RT_EINVAL;
    }

    cycle = g_mock_tick % 32U;

    sample->center_freq_mhz = g_sector_freq_mhz[sector_index];
    sample->noise_floor_q10 = 1024U;
    sample->rssi_q10 = 980U + sector_index * 24U;
    sample->kurtosis_q8 = 3U * APP_KURTOSIS_SCALE;

    if ((sector_index == 1U) && (cycle >= 8U) && (cycle <= 18U))
    {
        sample->rssi_q10 = 1880U;
        sample->kurtosis_q8 = 6U * APP_KURTOSIS_SCALE;
    }

    g_mock_tick++;
    return RT_EOK;
}
