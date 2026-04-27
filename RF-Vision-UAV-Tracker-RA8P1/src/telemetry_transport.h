#ifndef TELEMETRY_TRANSPORT_H
#define TELEMETRY_TRANSPORT_H

#include <rtthread.h>

rt_err_t telemetry_transport_init(void);
rt_err_t telemetry_transport_send(const char * payload);

#endif
