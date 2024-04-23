#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "gpio_setup_task.h"

#define LOG_MODULE_NAME         gpio_setup_task

LOG_MODULE_REGISTER(LOG_MODULE_NAME);



void gpio_handler(struct k_work *work)
{
    LOG_INF("My gpio work can be done from the gpio_setup_task.c !!!");
}








