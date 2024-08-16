#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/uart.h>
#include <uart_async_adapter.h>
#include <zephyr/sys/ring_buffer.h>

/* Defined in Kconfig menu */
#define UART_RING_BUF_DATA CONFIG_UART_BUFFER_SIZE

#define LOG_MODULE_NAME uart_service
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

ZBUS_CHAN_DECLARE(uart_chan);

/* Define the handler thread and its' message queue size */
ZBUS_SUBSCRIBER_DEFINE(uart_sub, 4);

/* Get the uart device that I want to use */
static const struct device *uart = DEVICE_DT_GET(DT_NODELABEL(uart0));

/* Create an uart async adaptor instance */
UART_ASYNC_ADAPTER_INST_DEFINE(async_adapter);

/* Compile time macro for declaring a ring buffer for use with raw bytes */
RING_BUF_DECLARE(uart_ring_buf, UART_RING_BUF_DATA);

/* Run time declaration of ring buffer */
// struct uart_ring_buffer
// {
//     struct ring_buf rb;
//     uint32_t buffer[UART_RING_BUF_DATA]
// };
// struct uart_ring_buffer uart_buffer;


/**
 * @brief Test if peripheral can do async api
 * 
 * @param dev Device pointer that points to the peripheral
 */
static bool uart_test_async_api(const struct device *dev);
static bool uart_test_async_api(const struct device *dev)
{
	const struct uart_driver_api *api =
        (const struct uart_driver_api *)dev->api;

	return (api->callback_set != NULL);
}

/**
 * @brief Handler thread for the UART service
 * 
 * @param void No input param
 */
static void uart_service(void)
{
    const struct zbus_channel *chan;

    while (!zbus_sub_wait(&uart_sub, &chan, K_FOREVER))
    {
        LOG_INF("Entered uart_service! Testing if the async adapter works...");

        int ret;
        int pos;
        struct uart_data_t *rx;
        struct uart_data_t *tx;

        if (!device_is_ready(uart))
        {
            return -ENODEV;
	    }
        if (!uart_test_async_api(uart))
        {
            /* Implement API adapter */
            uart_async_adapter_init(async_adapter, uart);
            uart = async_adapter;
        }
        LOG_INF("Success. Async API adapter works");

        // rx

        
    }
    


}
K_THREAD_DEFINE(uart_service_id, 1024, uart_service, NULL, NULL, NULL, 2, 0, 0);

