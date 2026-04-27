#ifndef RF_DETECTOR_SIMPLE_H
#define RF_DETECTOR_SIMPLE_H

#include "app_types.h"

void rf_detector_init(void);
void rf_detector_reset(void);
rt_bool_t rf_detector_process(const app_rf_sample_t * sample, app_detection_event_t * event);

#endif
