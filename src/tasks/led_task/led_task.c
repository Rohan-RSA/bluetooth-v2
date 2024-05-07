#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <gpio_setup_task/gpio_setup_task.h>
#include "led_task.h"

#define LOG_MODULE_NAME         led_task

LOG_MODULE_REGISTER(LOG_MODULE_NAME);


void led_handler(struct k_work *work) {

    /**
     * @brief Get address of struct led_work...
     * 
     */
    struct led_work_s *led_work = CONTAINER_OF(work, struct led_work_s, work);

    switch (led_work->led_action)
    {
    case STARTUP:
        
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

        LOG_INF("case STARTUP completed.");

        break;

    case POWERON:

        LOG_INF("case POWERON started.");
        gpio_pin_set_dt(&power_led, 0);
        gpio_pin_set_dt(&ble_led, 0);
        gpio_pin_set_dt(&conn_led, 0);

        LOG_INF("case POWERON must now toggle at 100ms.");
        gpio_pin_set_dt(&power_led, 1);
        k_msleep(100);
        gpio_pin_set_dt(&power_led, 0);
        LOG_INF("case POWERON end point.");
    
    default:
        break;
    }    


}


