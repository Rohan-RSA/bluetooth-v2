/**
 * @file main.c
 * @author Rohan Lourens
 * @brief 
 * @version 0.1
 * @date 2024-04-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <soc.h>
#include <stdio.h>
#include <stddef.h>
#include <zephyr/kernel.h>
#include <zephyr/types.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/pm/pm.h>
#include <zephyr/pm/device.h>
#include <zephyr/pm/device_runtime.h>
#include <zephyr/pm/state.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/sys/util_macro.h>
#include <zephyr/zbus/zbus.h>

#include <led_task/led_task.h>

#define PRIORITY        7
#define STACK_SIZE      2048
#define SLEEP_TIME      1000

#define LOG_MODULE_NAME         main
LOG_MODULE_REGISTER(LOG_MODULE_NAME);


void gpio_handler(struct k_work *work);
void led_handler(struct k_work *work);

K_WORK_DEFINE(gpio_worker, gpio_handler);
K_WORK_DEFINE(led_worker, led_handler);

static struct led_work_s led_work;

static const struct gpio_dt_spec pto_sensor_pin = GPIO_DT_SPEC_GET(DT_NODELABEL(button0), gpios);

static struct gpio_callback pto_cb_data;

void timer_1s_handler(struct k_timer *timer_1s)
{
        led_work.led_action =  POWERON;
        k_work_submit(&led_work.work);
}
K_TIMER_DEFINE(timer_1s, timer_1s_handler, NULL);

void pto_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
        LOG_INF("Callback works");
	// pto_logic_level_flag = gpio_pin_get_dt(&pto_sensor_pin);

	// LOG_INF("PTO input logic level = %d \n", pto_logic_level_flag);

	// pto_has_changed = true;
}


int main(void)
{
        int ret;

        LOG_INF("FT_BLE STARTING UP");

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

        k_work_submit(&gpio_worker);

        led_work.led_action =  STARTUP;
        k_work_init(&led_work.work, led_handler);
        k_work_submit(&led_work.work);

        k_timer_start(&timer_1s, K_SECONDS(2), K_SECONDS(2));
        
        return 0;
}

