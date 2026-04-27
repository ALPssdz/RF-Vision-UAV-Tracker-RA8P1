#include "telemetry_transport.h"

#define CONSOLE_CHUNK_SIZE 96U

rt_err_t telemetry_transport_console_init(void)
{
    rt_kprintf("[telemetry] transport=console\n");
    return RT_EOK;
}

rt_err_t telemetry_transport_console_send(const char * payload)
{
    const char * cursor;

    if (payload == RT_NULL)
    {
        return -RT_EINVAL;
    }

    cursor = payload;
    while (*cursor != '\0')
    {
        char chunk[CONSOLE_CHUNK_SIZE + 1U];
        rt_size_t len = 0;

        while ((len < CONSOLE_CHUNK_SIZE) && (cursor[len] != '\0'))
        {
            chunk[len] = cursor[len];
            len++;
        }
        chunk[len] = '\0';

        rt_kprintf("%s", chunk);
        cursor += len;
    }

    rt_kprintf("\n");
    return RT_EOK;
}

