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

void timer_1s_handler(struct k_timer *timer_1s)
{
        led_work.led_action =  POWERON;
        k_work_submit(&led_work.work);
}

K_TIMER_DEFINE(timer_1s, timer_1s_handler, NULL);


int main(void)
{
        LOG_INF("FT_BLE STARTING UP");

        led_work.led_action =  STARTUP;

        k_work_submit(&gpio_worker);

        k_work_init(&led_work.work, led_handler);
        k_work_submit(&led_work.work);

        led_work.led_action = POWERON;
        k_work_init(&led_work.work, led_handler);
        

        k_timer_start(&timer_1s, K_SECONDS(2), K_SECONDS(2));
        

        return 0;
}



