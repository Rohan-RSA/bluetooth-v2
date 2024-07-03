#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>

#include "advertise_task.h"
#include "led_task/led_task.h"

#define LOG_MODULE_NAME advertise_task

LOG_MODULE_REGISTER(LOG_MODULE_NAME);

ZBUS_CHAN_DECLARE(advertise_chan);

void wq_adv_cb(struct k_work *item)
{
    struct advertise_msg msg;
    struct wq_info *adv = CONTAINER_OF(item, struct wq_info, work);

    zbus_chan_read(adv->chan, &msg, K_MSEC(200));

    LOG_INF("Advertising msg processed by WORK QUEUE handler adv_cb%u: config advertising = %d, start advertising = %d, update advertising = %d, stop advertising = %d",
    adv->handle, msg.adv_config, msg.adv_start, msg.adv_update, msg.adv_stop);
    

};