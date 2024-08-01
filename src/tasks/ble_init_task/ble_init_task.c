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
	static struct bt_le_ext_adv *ft_adv;
	static struct advertise_msg adv_msg;

	while (!zbus_sub_wait(&ble_init_sub, &chan, K_FOREVER))
	{
		int ret;
		struct ble_init_msg msg;
		static struct advertise_sensor_type sensor_type;

		zbus_chan_read(chan, &msg, K_MSEC(200));
		zbus_chan_read(chan, &sensor_type, K_MSEC(200));
		LOG_INF("BLE init msg processed by thread ble_init_task: init = %s", msg.init ? "true":"false");
		LOG_INF("Sensor type processed by THREAD handler adv_init_sub:\r\npto = %d,\r\npressure = %d,\r\nflow = %d",
							sensor_type.pto, sensor_type.pressure, sensor_type.flow);
							/**
							 * I need the know the sensor type here because this determines what type of adv
							 * parameters I will use to configure. I have not yet 
							 */
		

		

		return;
	}
}
K_THREAD_DEFINE(ble_init_id, 1024, ble_init_task, NULL, NULL, NULL, 2, 0, 0);
