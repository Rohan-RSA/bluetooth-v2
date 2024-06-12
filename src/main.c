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
#include <gpio_setup_task/gpio_setup_task.h>

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

struct led_msg{
    int startupAction;
    int poweronAction;
    int advertisingAction;
    int errorAction;
};

ZBUS_CHAN_DEFINE( led_chan,
                  struct led_msg,
                  NULL,
                  NULL,
                  ZBUS_OBSERVERS(led_service_listener),
                  ZBUS_MSG_INIT(0)

);

void led_callback_listener(struct zbus_channel *chan)
{
        struct led_msg *msg = zbus_chan_const_msg(chan);
        // msg = zbus_chan_read(&led_chan, &led_lis_action, K_MSEC(500));
        LOG_INF("led_callback_listener is working");
        LOG_INF("led message is: \n\r startupAction = %d\n\r poweronAction = %d",
                 msg->startupAction, msg->poweronAction);
        // LOG_INF("%d", led_lis_action->startupAction);
};

ZBUS_LISTENER_DEFINE(led_service_listener, led_callback_listener);

void timer_1s_handler(struct k_timer *timer_1s)
{
        led_work.led_action =  POWERON;
        k_work_submit(&led_work.work);
}
K_TIMER_DEFINE(timer_1s, timer_1s_handler, NULL);

int main(void)
{
        int ret;

        struct led_msg led_lis_action = {
                .startupAction = 1,
                .poweronAction = 0,
                .advertisingAction = 0,
                .errorAction = 0
        };

        LOG_INF("FT_BLE STARTING UP");
        zbus_chan_pub(&led_chan, &led_lis_action, K_SECONDS(1));

        // k_work_submit(&gpio_worker);
        // led_work.led_action =  STARTUP;
        // k_work_init(&led_work.work, led_handler);
        // k_work_submit(&led_work.work);

        // k_timer_start(&timer_1s, K_SECONDS(2), K_SECONDS(2));
        
        return 0;
}
