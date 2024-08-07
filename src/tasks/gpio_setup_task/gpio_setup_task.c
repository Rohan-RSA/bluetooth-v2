#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/zbus/zbus.h>

#include "gpio_setup_task.h"
#include "advertise_task/advertise_task.h"
#include "ble_init_task/ble_init_task.h"

#define LOG_MODULE_NAME gpio_setup_task

LOG_MODULE_REGISTER(LOG_MODULE_NAME);

ZBUS_CHAN_DECLARE(ble_chan);
ZBUS_CHAN_DECLARE(led_chan);

const struct gpio_dt_spec power_led         = GPIO_DT_SPEC_GET(POWER_LED, gpios);
const struct gpio_dt_spec conn_led          = GPIO_DT_SPEC_GET(CONN_LED, gpios);
const struct gpio_dt_spec ble_led           = GPIO_DT_SPEC_GET(BLE_LED, gpios);

const struct gpio_dt_spec pto_sensor_pin    = GPIO_DT_SPEC_GET(DT_NODELABEL(button0), gpios);

const struct gpio_dt_spec rotary1           = GPIO_DT_SPEC_GET(DT_NODELABEL(button1), gpios);
const struct gpio_dt_spec rotary2           = GPIO_DT_SPEC_GET(DT_NODELABEL(button2), gpios);
const struct gpio_dt_spec rotary3           = GPIO_DT_SPEC_GET(DT_NODELABEL(button3), gpios);
const struct gpio_dt_spec rotary4           = GPIO_DT_SPEC_GET(DT_NODELABEL(button4), gpios);

struct gpio_callback pto_cb_data;

static struct advertise_msg adv_msg =
{
    .adv_config = 0,
    .adv_start = 0,
    .adv_stop = 0,
    .adv_update = 0,
    .adv_stop = 0,
    .pto = 0,
    .pressure = 0,
    .flow = 0,
    .sensor_state = 0
};

void pto_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    LOG_INF("PTO callback");

    adv_msg.adv_config = 0;
    adv_msg.adv_start = 0;
    adv_msg.adv_update = 1;
    adv_msg.sensor_state = gpio_pin_get_dt(&pto_sensor_pin);
    
    LOG_INF("adv_msg.sensor_state from gpio setup task = %d", adv_msg.sensor_state);
    zbus_chan_pub(&ble_chan, &adv_msg, K_NO_WAIT);
    // zbus_chan_notify(&ble_chan, K_NO_WAIT);
}

void gpio_handler(struct k_work *work)
{
    int ret;

    LOG_INF("Initializng GPIO");

    ret = gpio_pin_configure_dt(&power_led, GPIO_OUTPUT);
    if (ret != 0)
    {
        LOG_ERR("Could not configure the power LED");
        return;
    }  
    ret = gpio_pin_configure_dt(&conn_led, GPIO_OUTPUT);
    if (ret != 0)
    {
        LOG_ERR("Could not configure the connection LED");
        return;
    }  
    ret = gpio_pin_configure_dt(&ble_led, GPIO_OUTPUT);
    if (ret != 0)
    {
        LOG_ERR("Could not configure the ble LED");
        return;
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
    ret = gpio_pin_configure_dt(&rotary1, GPIO_INPUT);
    if (ret != 0)
    {
        LOG_ERR("Could not configure rotary pin 1 as input.");
        return;
    }
    ret = gpio_pin_configure_dt(&rotary2, GPIO_INPUT);
    if (ret != 0)
    {
        LOG_ERR("Could not configure rotary pin 2 as input.");
        return;
    }
    ret = gpio_pin_configure_dt(&rotary3, GPIO_INPUT);
    if (ret != 0)
    {
        LOG_ERR("Could not configure rotary pin 3 as input.");
        return;
    }
    // ret = gpio_pin_configure_dt(&rotary4, GPIO_INPUT);
    // if (ret != 0)
    // {
    //     LOG_ERR("Could not configure rotary pin 4 as input.");
    //     return;
    // } Unsupported pin!!!!!
    LOG_INF("Initializng GPIO completed");

    gpio_init_callback(&pto_cb_data, pto_callback, BIT(pto_sensor_pin.pin));
    gpio_add_callback(pto_sensor_pin.port, &pto_cb_data);
}

void rotary_handler(struct k_work *work)
{
    uint8_t ret;
    uint8_t r_input_1, r_input_2, r_input_3;

    LOG_INF("Entered rotary switch work handler");

	// Check each pin state
	r_input_1 = gpio_pin_get_dt(&rotary1);
	r_input_2 = gpio_pin_get_dt(&rotary2);
	r_input_3 = gpio_pin_get_dt(&rotary3);
	// r_input_4 = gpio_pin_get_dt(&rotary4); unsupported pin!!!

    LOG_INF("Rotary switch binary output: %u %u %u", r_input_1, r_input_2, r_input_3);

    if (r_input_1 && r_input_2 && r_input_3 == 1)
	{
		LOG_INF("Rotary switch set to pto");
        adv_msg.pto = 1;
        adv_msg.adv_config = 1;

        ret = zbus_chan_pub(&ble_chan, &adv_msg, K_MSEC(200));
        if (ret != 0)
        {
            LOG_ERR("Could not publish adv_msg to ble channel");
            return 0;
        }
	}
	else if ((r_input_1 == 0) && (r_input_2 && r_input_3 == 1))
	{
		LOG_INF("Rotary switch set to pressure");
        adv_msg.pressure = 1;
        adv_msg.adv_config = 1;

        ret = zbus_chan_pub(&ble_chan, &adv_msg, K_MSEC(200));
        if (ret != 0)
        {
            LOG_ERR("Could not publish adv_msg to ble channel");
            return 0;
        }
	}
	else if ((r_input_2 == 0) && (r_input_1 && r_input_3 == 1))
	{
		LOG_INF("Rotary switch set to flow");
        adv_msg.flow = 1;
        adv_msg.adv_config = 1;

        ret = zbus_chan_pub(&ble_chan, &adv_msg, K_MSEC(200));
        if (ret != 0)
        {
            LOG_ERR("Could not publish adv_msg to ble channel");
            return 0;
        }
	}
	LOG_INF("Exiting rotary sensor select thread.");
}
