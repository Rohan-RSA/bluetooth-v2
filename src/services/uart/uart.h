#pragma once

#include <stdbool.h>
#include <zephyr/types.h>

#define UART_BUF_SIZE CONFIG_UART_BUFFER_SIZE

static struct uart_msg
{
    bool init;
};

struct uart_data_t
{
    void *fifo_reserved;
    uint8_t data[UART_BUF_SIZE];
    uint16_t len;
};
