#ifndef GPIO_SETUP_TASK_H
#define GPIO_SETUP_TASK_H

// #pragma once

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define POWER_LED       DT_ALIAS(led0)
#define CONN_LED        DT_ALIAS(led1)
#define BLE_LED         DT_ALIAS(led2)
// #define BLE_CON_LED     DT_ALIAS(led3)
// #define BLE_LED         DT_ALIAS(led4)

extern const struct gpio_dt_spec power_led;
extern const struct gpio_dt_spec conn_led;
extern const struct gpio_dt_spec ble_led;
// extern const struct gpio_dt_spec ble_con_led;
// extern const struct gpio_dt_spec ble_led;
extern struct gpio_callback pto_cb_data;
extern const struct gpio_dt_spec pto_sensor_pin;

void gpio_handler(struct k_work *work);

void pto_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins);

#endif /* GPIO_SETUP_TASK_H */
