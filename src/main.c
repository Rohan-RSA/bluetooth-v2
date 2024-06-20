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

// Hello World

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

// This is the sys work queue async callback/handler function
static void wq_dh_cb(struct k_work *item)
{
        struct led_msg msg;
        struct wq_info *led = CONTAINER_OF(item, struct wq_info, work);

        zbus_chan_read(led->chan, &msg, K_MSEC(200));

        LOG_INF("LED msg processed by WORK QUEUE handler dh%u: startup action = %d, power on action = %d,advertising action = %d, error action = %d",
        led->handle, msg.startupAction, msg.poweronAction, msg.advertisingAction, msg.errorAction);

        if (msg.startupAction == 1)
        {
                for (size_t i = 0; i < 3; i++)
                {
                        gpio_pin_toggle_dt(&power_led);
                        k_msleep(100);
                        gpio_pin_toggle_dt(&conn_led);
                        k_msleep(100);
                        gpio_pin_toggle_dt(&ble_led);
                        k_msleep(100);
                        gpio_pin_toggle_dt(&ble_led);
                        k_msleep(100);
                        gpio_pin_toggle_dt(&conn_led);
                        k_msleep(100);
                        gpio_pin_toggle_dt(&power_led);
                        k_msleep(100);
                }

                gpio_pin_set_dt(&power_led, 0);
                gpio_pin_set_dt(&ble_led, 0);
                gpio_pin_set_dt(&conn_led, 0); 
                led_task.startupAction = 0; 
        }
        if (msg.poweronAction == 1)
        {
                // Here I would like to use the LED API to continually blink this led forever
                gpio_pin_set_dt(&power_led, 1);
                k_msleep(20);
                gpio_pin_set_dt(&power_led, 0);
        }

};

static void dh1_cb(const struct zbus_channel *chan)
{
        wq_led_handler1.chan = chan;
        k_work_submit(&wq_led_handler1.work);
}
ZBUS_LISTENER_DEFINE(delay_handler1_lis, dh1_cb);

void timer_1s_handler(struct k_timer *timer_1s)
{
        // led_task.startupAction = 0;
        // zbus_chan_pub(&led_chan, &led_task, K_NO_WAIT);
        // k_work_submit(&wq_led_handler1.work);
        
        // led_task.poweronAction = 1;
        // led_work.led_task =  POWERON;
        // k_work_submit(&led_work.work);
}
K_TIMER_DEFINE(timer_1s, timer_1s_handler, NULL);

int main(void)
{
        LOG_INF("FT_BLE STARTING UP");

        int ret;

        k_work_submit(&gpio_worker);

        // Send message to do the start-up LED sequence (this is synchronous and blocking)
        // zbus_chan_pub(&led_chan, &led_task, K_SECONDS(1));
        
        k_work_init(&wq_led_handler1.work, wq_dh_cb);
        
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
