#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "led_task.h"

#define LOG_MODULE_NAME         led_task

LOG_MODULE_REGISTER(LOG_MODULE_NAME);


void led_handler(struct k_work *work)
{
    LOG_INF("My led work can be done from the led_task.c !!!");
}