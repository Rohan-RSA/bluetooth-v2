#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <gpio_setup_task/gpio_setup_task.h>
#include "led_task.h"

#define LOG_MODULE_NAME         led_task

LOG_MODULE_REGISTER(LOG_MODULE_NAME);


void led_handler(struct k_work *work)
{
    LOG_INF("LED tasks:");

    gpio_pin_set_dt(&power_led, 1);
    gpio_pin_set_dt(&rx_led, 1);
    gpio_pin_set_dt(&tx_led, 1);
    // gpio_pin_set_dt(&ble_con_led, 1);
    // gpio_pin_set_dt(&ble_led, 1);
}