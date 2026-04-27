#ifndef TELEMETRY_H
#define TELEMETRY_H

#include "app_types.h"

void telemetry_init(void);
void telemetry_send_boot(void);
void telemetry_send_status(const app_detection_event_t * event);
void telemetry_send_alert(const app_detection_event_t * event);

#endif
