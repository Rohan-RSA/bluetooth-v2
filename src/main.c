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
// #include <zephyr/drivers/led.h>
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
#include <gpio_setup_task/gpio_setup_task.h>

#define PRIORITY        7
#define STACK_SIZE      2048
#define SLEEP_TIME      1000

#define LOG_MODULE_NAME         main
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

void gpio_handler(struct k_work *work);
// void led_handler(struct k_work *work);

K_WORK_DEFINE(gpio_worker, gpio_handler);
// K_WORK_DEFINE(led_worker, led_handler);

// static struct led_work_s led_work;

struct led_msg led_task = {
        .startupAction = 1,
        .poweronAction = 1,
        .advertisingAction = 0,
        .errorAction = 0
};

static struct wq_info wq_led_handler1 = {.handle = 1};
// static struct wq_info wq_led_handler2 = {.handle = 2};
// static struct wq_info wq_led_handler3 = {.handle = 3};

ZBUS_CHAN_DEFINE(led_chan,
                struct led_msg,
                NULL,
                NULL,
                ZBUS_OBSERVERS(delay_handler1_lis),
                ZBUS_MSG_INIT(0)
);
// led_service_listener, 

void timer_1s_handler(struct k_timer *timer_1s)
{
        led_task.startupAction = 0;
        led_task.poweronAction = 1;
        led_task.errorAction = 0;
        led_task.advertisingAction = 0;

        zbus_chan_pub(&led_chan, &led_task, K_NO_WAIT);

        // k_work_submit(&wq_led_handler1.work);
}
K_TIMER_DEFINE(timer_1s, timer_1s_handler, NULL);

int main(void)
{
        LOG_INF("FT_BLE STARTING UP");

        int ret;

        // Once off task
        k_work_submit(&gpio_worker);

        // Send message to do the start-up LED sequence (this is synchronous and blocking?)
        // zbus_chan_pub(&led_chan, &led_task, K_SECONDS(1));
        
        k_work_init(&wq_led_handler1.work, wq_led_cb);
        
        ret = zbus_chan_pub(&led_chan, &led_task, K_MSEC(200));
        if (ret != 0)
        {
                LOG_ERR("Could not publish to led channel");
        }
        
        // led_work.led_task =  STARTUP;
        // k_work_init(&led_work.work, led_handler);
        // k_work_submit(&led_work.work);

        k_timer_start(&timer_1s, K_SECONDS(2), K_SECONDS(2));
        
        return 0;
}
