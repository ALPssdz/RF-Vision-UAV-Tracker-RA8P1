#ifndef APP_TYPES_H
#define APP_TYPES_H

#include <rtthread.h>

typedef enum
{
    APP_RF_BAND_5745 = 0,
    APP_RF_BAND_5785,
    APP_RF_BAND_5825,
} app_rf_band_t;

typedef struct
{
    rt_uint32_t center_freq_mhz;
    rt_uint32_t rssi_q10;
    rt_uint32_t noise_floor_q10;
    rt_uint16_t kurtosis_q8;
} app_rf_sample_t;

typedef struct
{
    rt_bool_t detected;
    rt_uint32_t center_freq_mhz;
    rt_uint32_t score_q10;
    rt_uint8_t confidence;
    rt_uint8_t confirm_count;
} app_detection_event_t;

#endif
