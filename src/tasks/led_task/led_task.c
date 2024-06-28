#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <gpio_setup_task/gpio_setup_task.h>

#include "led_task.h"

#define LOG_MODULE_NAME         led_task

LOG_MODULE_REGISTER(LOG_MODULE_NAME);

ZBUS_CHAN_DECLARE(led_chan);

static struct wq_info wq_led_handler1 = {.handle = 1};

// This is the sys work queue async callback/handler function
void wq_led_cb(struct k_work *item)
{
        struct led_msg msg;
        struct wq_info *led = CONTAINER_OF(item, struct wq_info, work);

        zbus_chan_read(led->chan, &msg, K_MSEC(200));

        LOG_DBG("LED msg processed by WORK QUEUE handler dh%u: startup action = %d, power on action = %d,advertising action = %d, error action = %d",
        led->handle, msg.startupAction, msg.poweronAction, msg.advertisingAction, msg.errorAction);
        // LOG_INF("The work item received is %d", led->work);

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
                msg.startupAction = 0; 
        }
        if (msg.poweronAction == 1)
        {
                // Here I would like to use the LED API to continually blink this led forever
                gpio_pin_set_dt(&power_led, 1);
                k_msleep(20);
                gpio_pin_set_dt(&power_led, 0);
        }

};

void dh1_cb(const struct zbus_channel *chan)
{
        wq_led_handler1.chan = chan;
        k_work_submit(&wq_led_handler1.work);
}
ZBUS_LISTENER_DEFINE(delay_handler1_lis, dh1_cb);

// void led_callback_listener(struct zbus_channel *chan)
// {
//     int ret;
//     struct led_msg *msg = zbus_chan_const_msg(chan);

//     LOG_INF("led message is: \n\r startupAction = %d\n\r poweronAction = %d",
//                 msg->startupAction, msg->poweronAction);

//     if (msg->startupAction == 1)
//     {
//         for (size_t i = 0; i < 3; i++)
//         {
//             ret = gpio_pin_toggle_dt(&power_led);
//             k_msleep(100);
//             gpio_pin_toggle_dt(&conn_led);
//             k_msleep(100);
//             gpio_pin_toggle_dt(&ble_led);
//             k_msleep(100);
//             gpio_pin_toggle_dt(&ble_led);
//             k_msleep(100);
//             gpio_pin_toggle_dt(&conn_led);
//             k_msleep(100);
//             gpio_pin_toggle_dt(&power_led);
//             k_msleep(100);
//         }
//         gpio_pin_set_dt(&power_led, 0);
//         gpio_pin_set_dt(&ble_led, 0);
//         gpio_pin_set_dt(&conn_led, 0);

//         msg->poweronAction = 0;
//     }

//     if (msg->poweronAction == 1)
//     {
//         gpio_pin_set_dt(&power_led, 1);
//         k_msleep(20);
//         gpio_pin_set_dt(&power_led, 0);
//     }

// };
// ZBUS_LISTENER_DEFINE(led_service_listener, led_callback_listener);
