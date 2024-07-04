#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/hci.h>

#include "advertise_task.h"
#include "led_task/led_task.h"

#define LOG_MODULE_NAME advertise_task

LOG_MODULE_REGISTER(LOG_MODULE_NAME);

ZBUS_CHAN_DECLARE(ble_chan);

struct bt_le_adv_param ft_params = BT_LE_ADV_PARAM_INIT( BT_LE_ADV_OPT_USE_IDENTITY |
                                                         BT_LE_ADV_OPT_EXT_ADV | BT_LE_ADV_OPT_CODED,
                                                         BT_GAP_ADV_SLOW_INT_MIN,
                                                         BT_GAP_ADV_SLOW_INT_MAX,
                                                         NULL);

struct bt_le_ext_adv *ft_adv;

void wq_adv_cb(struct k_work *item)
{
    int ret;
    const struct advertise_msg msg;
    struct wq_info *adv = CONTAINER_OF(item, struct wq_info, work);

    static struct led_msg; //skuif die nou nou na header

    static struct led_msg led_task = 
    {
        .startupAction = 0,
        .poweronAction = 0,
        .advertisingAction = 0,
        .errorAction = 0
    };

    zbus_chan_read(adv->chan, &msg, K_MSEC(200));

    LOG_INF("Advertising msg processed by WORK QUEUE handler wq_adv_cb with handle %u: config advertising = %d, start advertising = %d, update advertising = %d, stop advertising = %d",
    adv->handle, msg.adv_config, msg.adv_start, msg.adv_update, msg.adv_stop);
    
    ret = bt_enable(NULL);
    if (ret != 0)
    {
        LOG_INF("Bluetooth init failed (err %d)", ret);

        led_task.errorAction = 1;
     
        ret = zbus_chan_pub(&ble_chan, &led_task, K_MSEC(200));
        if (ret != 0)
        {
            LOG_ERR("Could not publish error led task to ble channel");
            return 0;
        }

        LOG_INF("Published led error task to ble channel");
    }

    LOG_INF("BLE init completed.");

    led_task.advertisingAction = 1;
    ret = zbus_chan_pub(&ble_chan, &led_task, K_MSEC(200));
    if (ret != 0)
    {
        LOG_ERR("Could not publish to ble channel");
        return 0;
    }
    
    ret = bt_le_ext_adv_create(&ft_params, NULL, &ft_adv);
    if (ret)
    {
        LOG_ERR("Failed to create advertiser set (err %d) \n", ret);
        return ret;
    }
    LOG_INF("Created extended advertising set ft_adv: %p", (void*) ft_adv);
    

};