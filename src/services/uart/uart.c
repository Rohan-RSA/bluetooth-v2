#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/uart.h>
#include <uart_async_adapter.h>
#include <stdio.h>
#include <stddef.h>
#include <zephyr/types.h>
#include "uart.h"


ZBUS_CHAN_DECLARE(uart_chan);
/* Define the handler thread and its' message queue size */
ZBUS_SUBSCRIBER_DEFINE(uart_sub, 4);

/* Defined in Kconfig menu */
#define UART_BUF_DATA CONFIG_UART_BUFFER_SIZE

#define LOG_MODULE_NAME uart_service
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)
#define MSG_SIZE	32

/* queue to store up to 10 messages (aligned to 4-byte boundary) */
K_MSGQ_DEFINE(uart_msgq, MSG_SIZE, 10, 4);

/* Get the uart device that I want to use */
static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

/* receive buffer used in UART ISR callback */
static char rx_buf[MSG_SIZE];
static int rx_buf_pos;

void serial_cb(const struct device *dev, void *user_data)
{
	uint8_t c;

	if (!uart_irq_update(uart_dev)) {
		return;
	}

	if (!uart_irq_rx_ready(uart_dev)) {
		return;
	}

	/* read until FIFO empty */
	while (uart_fifo_read(uart_dev, &c, 1) == 1) {
		if ((c == '\n' || c == '\r') && rx_buf_pos > 0) {
			/* terminate string */
			rx_buf[rx_buf_pos] = '\0';

			/* if queue is full, message is silently dropped */
			k_msgq_put(&uart_msgq, &rx_buf, K_NO_WAIT);

			/* reset the buffer (it was copied to the msgq) */
			rx_buf_pos = 0;
		} else if (rx_buf_pos < (sizeof(rx_buf) - 1)) {
			rx_buf[rx_buf_pos++] = c;
		}
		/* else: characters beyond buffer size are dropped */
	}
}

/*
 * Print a null-terminated string character by character to the UART interface
 */
void print_uart(char *buf)
{
	int msg_len = strlen(buf);

	for (int i = 0; i < msg_len; i++) {
		uart_poll_out(uart_dev, buf[i]);
	}
}

static void uart_service(void)
{
    const struct zbus_channel *chan;


    while (!zbus_sub_wait(&uart_sub, &chan, K_FOREVER))
    {
        LOG_INF("Entered uart_service!");

		char tx_buf[MSG_SIZE];
		
		if (!device_is_ready(uart_dev))
		{
			LOG_ERR("UART device not found");
			return 0;
		}

		/* Configure interrupt and callback to receive data */
		int ret = uart_irq_callback_user_data_set(uart_dev, serial_cb, NULL);

		if (ret < 0)
		{
			if (ret == -ENOTSUP)
			{
				LOG_ERR("Interrupt-driven UArt API support not enabled");
			}
			else if (ret == -ENOSYS)
			{
				LOG_ERR("UART device does not support interrupt-driven API");
			}
			else LOG_ERR("Error settting UART callback: %d", ret);

			return 0;			
		}

		uart_irq_rx_enable(uart_dev);
		print_uart("Testing if uart is working?\r\n");
		print_uart("Enter something and press enter:\r\n");
		
        
		/* indefinitely wait for input from the user */
		while (k_msgq_get(&uart_msgq, &tx_buf, K_FOREVER) == 0) {
			print_uart("Echo: ");
			print_uart(tx_buf);
			print_uart("\r\n");
		}
		return 0;
		
    }
}
K_THREAD_DEFINE(uart_service_id, 1024, uart_service, NULL, NULL, NULL, 1, 0, 0);
