#include "telemetry_transport.h"

#include "app_config.h"

#if (APP_TELEMETRY_TRANSPORT == APP_TELEMETRY_TRANSPORT_UDP) && defined(RT_USING_SAL)
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

static int g_udp_socket = -1;
static struct sockaddr_in g_center_addr;

rt_err_t telemetry_transport_udp_init(void)
{
    g_udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (g_udp_socket < 0)
    {
        rt_kprintf("[telemetry] udp socket create failed\n");
        return -RT_ERROR;
    }

    rt_memset(&g_center_addr, 0, sizeof(g_center_addr));
    g_center_addr.sin_family = AF_INET;
    g_center_addr.sin_port = htons(APP_TELEMETRY_CENTER_PORT);
    g_center_addr.sin_addr.s_addr = inet_addr(APP_TELEMETRY_CENTER_HOST);

    rt_kprintf("[telemetry] transport=udp target=%s:%u\n",
               APP_TELEMETRY_CENTER_HOST,
               APP_TELEMETRY_CENTER_PORT);
    return RT_EOK;
}

rt_err_t telemetry_transport_udp_send(const char * payload)
{
    int sent;
    rt_size_t len;

    if ((payload == RT_NULL) || (g_udp_socket < 0))
    {
        return -RT_EINVAL;
    }

    len = rt_strlen(payload);
    sent = sendto(g_udp_socket,
                  payload,
                  len,
                  0,
                  (struct sockaddr *)&g_center_addr,
                  sizeof(g_center_addr));
    return (sent == (int)len) ? RT_EOK : -RT_ERROR;
}

#else

rt_err_t telemetry_transport_udp_init(void)
{
    rt_kprintf("[telemetry] transport=udp target=%s:%u (stub)\n",
               APP_TELEMETRY_CENTER_HOST,
               APP_TELEMETRY_CENTER_PORT);
    rt_kprintf("[telemetry] enable RT_USING_SAL/lwIP before switching APP_TELEMETRY_TRANSPORT to UDP\n");
    return -RT_ENOSYS;
}

rt_err_t telemetry_transport_udp_send(const char * payload)
{
    RT_UNUSED(payload);
    return -RT_ENOSYS;
}

#endif
