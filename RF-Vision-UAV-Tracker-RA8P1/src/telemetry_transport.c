#include "telemetry_transport.h"

#include "app_config.h"

rt_err_t telemetry_transport_console_init(void);
rt_err_t telemetry_transport_console_send(const char * payload);
rt_err_t telemetry_transport_udp_init(void);
rt_err_t telemetry_transport_udp_send(const char * payload);

rt_err_t telemetry_transport_init(void)
{
#if APP_TELEMETRY_TRANSPORT == APP_TELEMETRY_TRANSPORT_UDP
    return telemetry_transport_udp_init();
#else
    return telemetry_transport_console_init();
#endif
}

rt_err_t telemetry_transport_send(const char * payload)
{
#if APP_TELEMETRY_TRANSPORT == APP_TELEMETRY_TRANSPORT_UDP
    return telemetry_transport_udp_send(payload);
#else
    return telemetry_transport_console_send(payload);
#endif
}
