#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/hci.h>

#include <ble_init_task/ble_init_task.h>

#define LOG_MODULE_NAME ble_init_task
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

ZBUS_CHAN_DECLARE(ble_chan);

ZBUS_SUBSCRIBER_DEFINE(ble_init_sub, 4);

static void ble_init_task(void)
{
    const struct zbus_channel *chan;

    while (!zbus_sub_wait(&ble_init_sub, &chan, K_FOREVER))
    {
        int ret;
        struct ble_init_msg msg;

        zbus_chan_read(chan, &msg, K_MSEC(200));

        LOG_INF("BLE init msg processed by thread ble_init_task: init = %s", msg.init ? "true":"false");
        return;
    }

    


}
K_THREAD_DEFINE(ble_init_id, 1024, ble_init_task, NULL, NULL, NULL, 3, 0, 0);

