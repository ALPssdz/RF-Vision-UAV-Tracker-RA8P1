#ifndef SDR_FRONTEND_H
#define SDR_FRONTEND_H

#include <rtthread.h>

#include "app_types.h"

rt_err_t sdr_frontend_init(void);
rt_err_t sdr_frontend_read(rt_uint32_t sector_index, app_rf_sample_t * sample);

#endif
