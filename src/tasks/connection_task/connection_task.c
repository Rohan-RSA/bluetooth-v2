#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/hci.h>

#include <connection_task/connection_task.h>
#include <advertise_task/advertise_task.h>

#define LOG_MODULE_NAME connection_task
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

ZBUS_CHAN_DECLARE(ble_chan);

ZBUS_SUBSCRIBER_DEFINE(connection_sub, 4);

static void connection_task(void)
{
	const struct zbus_channel *chan;
	// static struct bt_le_ext_adv *ft_adv;
	// static struct advertise_msg adv_msg;

	while (!zbus_sub_wait(&connection_sub, &chan, K_FOREVER))
	{
		// put connection/message struct here

		if (&ble_chan == chan)
		{
			LOG_INF("Entered connection_task");





		}
		

	}
}
K_THREAD_DEFINE(conn_task_id, 1024, connection_task, NULL, NULL, NULL, 2, 0, 0);
