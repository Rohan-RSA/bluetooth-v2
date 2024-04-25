#pragma once

#include <zephyr/kernel.h>

#define POWER_LED       DT_ALIAS(led0)
#define RX_LED          DT_ALIAS(led1)
#define TX_LED          DT_ALIAS(led2)
#define BLE_CON_LED     DT_ALIAS(led3)
#define BLE_LED         DT_ALIAS(led4)


extern const struct gpio_dt_spec power_led;
extern const struct gpio_dt_spec rx_led;
extern const struct gpio_dt_spec tx_led;
extern const struct gpio_dt_spec ble_con_led;
extern const struct gpio_dt_spec ble_led;

void gpio_handler(struct k_work *work);
