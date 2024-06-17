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


// void led_handler(struct k_work *work) {

//     /**
//      * @brief Get address of struct led_work...
//      * 
//      */
//     struct led_work_s *led_work = CONTAINER_OF(work, struct led_work_s, work);

//     switch (led_work->led_action)
//     {
//     case STARTUP:
        
//         for (size_t i = 0; i < 3; i++)
//         {
//             gpio_pin_toggle_dt(&power_led);
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

//         LOG_INF("case STARTUP completed.");

//         break;

//     case POWERON:

//         gpio_pin_set_dt(&power_led, 1);
//         k_msleep(20);
//         gpio_pin_set_dt(&power_led, 0);
        
//         break;


//     default:
//         break;
//     }    


// }


