#include "rf_detector_simple.h"

#include "app_config.h"

static rt_int16_t g_streak_q4[APP_SECTOR_COUNT];
static app_detection_event_t g_last_event;

void rf_detector_init(void)
{
    rf_detector_reset();
}

void rf_detector_reset(void)
{
    rt_memset(g_streak_q4, 0, sizeof(g_streak_q4));
    rt_memset(&g_last_event, 0, sizeof(g_last_event));
}

static rt_uint32_t sector_index_from_freq(rt_uint32_t freq_mhz)
{
    switch (freq_mhz)
    {
    case 5745U:
        return 0U;
    case 5785U:
        return 1U;
    case 5825U:
        return 2U;
    default:
        return 0U;
    }
}

static rt_uint32_t calc_score_q10(const app_rf_sample_t * sample)
{
    rt_uint32_t noise = sample->noise_floor_q10;
    rt_uint32_t energy_score;
    rt_uint32_t kurt_score = 0;

    if (noise < 1U)
    {
        noise = 1U;
    }

    energy_score = (sample->rssi_q10 * 1024U) / noise;

    if (sample->kurtosis_q8 > (3U * APP_KURTOSIS_SCALE))
    {
        kurt_score = ((rt_uint32_t)(sample->kurtosis_q8 - 3U * APP_KURTOSIS_SCALE) * 256U) /
                     APP_KURTOSIS_SCALE;
    }

    return energy_score + kurt_score;
}

rt_bool_t rf_detector_process(const app_rf_sample_t * sample, app_detection_event_t * event)
{
    rt_uint32_t score_q10;
    rt_uint32_t sector_index;
    rt_uint8_t confirm_count;

    if ((sample == RT_NULL) || (event == RT_NULL))
    {
        return RT_FALSE;
    }

    score_q10 = calc_score_q10(sample);
    sector_index = sector_index_from_freq(sample->center_freq_mhz);

    if (score_q10 >= APP_SCORE_THRESHOLD_Q10)
    {
        if (g_streak_q4[sector_index] < (rt_int16_t)(APP_CONFIRM_REQUIRED * APP_STREAK_UNIT_Q4))
        {
            g_streak_q4[sector_index] += APP_STREAK_UNIT_Q4;
        }
    }
    else if (g_streak_q4[sector_index] > 0)
    {
        g_streak_q4[sector_index] -= APP_STREAK_DECAY_Q4;
        if (g_streak_q4[sector_index] < 0)
        {
            g_streak_q4[sector_index] = 0;
        }
    }

    confirm_count = (rt_uint8_t)((g_streak_q4[sector_index] + APP_STREAK_UNIT_Q4 - 1U) /
                                  APP_STREAK_UNIT_Q4);

    event->detected = (confirm_count >= APP_CONFIRM_REQUIRED) ? RT_TRUE : RT_FALSE;
    event->center_freq_mhz = sample->center_freq_mhz;
    event->score_q10 = score_q10;
    event->confirm_count = confirm_count;
    event->confidence = (score_q10 >= 3072U) ? 100U : (rt_uint8_t)((score_q10 * 100U) / 3072U);

    g_last_event = *event;
    return event->detected;
}
