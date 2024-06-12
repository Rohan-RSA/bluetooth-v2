#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include "gpio_setup_task.h"

#define LOG_MODULE_NAME         gpio_setup_task

LOG_MODULE_REGISTER(LOG_MODULE_NAME);

const struct gpio_dt_spec power_led     = GPIO_DT_SPEC_GET(POWER_LED, gpios);
const struct gpio_dt_spec conn_led      = GPIO_DT_SPEC_GET(CONN_LED, gpios);
const struct gpio_dt_spec ble_led       = GPIO_DT_SPEC_GET(BLE_LED, gpios);
const struct gpio_dt_spec pto_sensor_pin = GPIO_DT_SPEC_GET(DT_NODELABEL(button0), gpios);
struct gpio_callback pto_cb_data;

void pto_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    LOG_INF("Callback works");  
}

void gpio_handler(struct k_work *work)
{
    int ret;

    LOG_INF("Initializng GPIO");

    ret = gpio_pin_configure_dt(&power_led, GPIO_OUTPUT);
    if (ret != 0)
    {
        LOG_ERR("Could not configure the power LED");
    }  
    ret = gpio_pin_configure_dt(&conn_led, GPIO_OUTPUT);
    if (ret != 0)
    {
        LOG_ERR("Could not configure the connection LED");
    }  
    ret = gpio_pin_configure_dt(&ble_led, GPIO_OUTPUT);
    if (ret != 0)
    {
        LOG_ERR("Could not configure the ble LED");
    }  

    // Configure pin 16 as an input pin
    ret = gpio_pin_configure_dt(&pto_sensor_pin, GPIO_INPUT);
    if (ret != 0)
    {
        LOG_ERR("Could not configure pto pin 16 as input.");
        return;
    }

    // Configure interrupt on pin 16
    ret = gpio_pin_interrupt_configure_dt(&pto_sensor_pin, GPIO_INT_EDGE_BOTH);
    if (ret != 0)
    {
        LOG_ERR("Could not configure interrupt on pin 16.");
        return;
    }

    // Check is port 0 is ready
    if (!device_is_ready(pto_sensor_pin.port))
    {
        LOG_ERR("GPIO port 0 is not ready");
        return;
    }

    gpio_init_callback(&pto_cb_data, pto_callback, BIT(pto_sensor_pin.pin));
    gpio_add_callback(pto_sensor_pin.port, &pto_cb_data);

    
}








