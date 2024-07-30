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
#include <advertise_task/advertise_task.h>

#define LOG_MODULE_NAME ble_init_task
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

ZBUS_CHAN_DECLARE(ble_chan);

ZBUS_SUBSCRIBER_DEFINE(ble_init_sub, 4);

static void ble_init_task(void)
{
	const struct zbus_channel *chan;
	struct bt_le_ext_adv *ft_adv;

	while (!zbus_sub_wait(&ble_init_sub, &chan, K_FOREVER))
	{
		int ret;
		struct ble_init_msg msg;

		zbus_chan_read(chan, &msg, K_MSEC(200));
		LOG_INF("BLE init msg processed by thread ble_init_task: init = %s", msg.init ? "true":"false");
		LOG_INF("Sensor type processed by THREAD handler adv_init_sub:\r\npto = %d,\r\npressure = %d,\r\nflow = %d",
					sensor_type.pto, sensor_type.pressure, sensor_type.flow);
		
    const struct bt_le_adv_param ft_params = BT_LE_ADV_PARAM_INIT(BT_LE_ADV_OPT_USE_IDENTITY |
                                                                  BT_LE_ADV_OPT_EXT_ADV | BT_LE_ADV_OPT_CODED,
                                                                  BT_GAP_ADV_SLOW_INT_MIN,
                                                                  BT_GAP_ADV_SLOW_INT_MAX,
                                                                  NULL);
		
		ret = bt_enable(NULL);
		if (ret != 0) LOG_ERR("Bluetooth init failed (err %d)", ret);
		
		if (IS_ENABLED(CONFIG_SETTINGS))
		{
			settings_load();
		}
		LOG_INF("BLE enable completed.");

		if (bt_is_ready)
		{
			ret = bt_le_ext_adv_create(&ft_params, NULL, &ft_adv);
			if (ret != 0)
			{
				LOG_ERR("Failed to create advertiser set (err %d)", ret);
				return ret;
			}
			LOG_INF("Created extended advertising set ft_adv: %p", (void*) ft_adv);
		}


		return;
	}

}
K_THREAD_DEFINE(ble_init_id, 1024, ble_init_task, NULL, NULL, NULL, 2, 0, 0);

