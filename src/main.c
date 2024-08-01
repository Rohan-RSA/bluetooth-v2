/**
 * @file main.c
 * @author Rohan Lourens
 * @brief 
 * @version 0.1
 * @date 2024-04-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <soc.h>
#include <stdio.h>
#include <stddef.h>
#include <zephyr/kernel.h>
#include <zephyr/types.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
// #include <zephyr/drivers/led.h>
#include <zephyr/logging/log.h>
#include <zephyr/pm/pm.h>
#include <zephyr/pm/device.h>
#include <zephyr/pm/device_runtime.h>
#include <zephyr/pm/state.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/sys/util_macro.h>
#include <zephyr/zbus/zbus.h>

#include <led_task/led_task.h>
#include <gpio_setup_task/gpio_setup_task.h>
#include <advertise_task/advertise_task.h>
#include <ble_init_task/ble_init_task.h>

#define PRIORITY        7
#define STACK_SIZE      2048
#define SLEEP_TIME      1000

#define LOG_MODULE_NAME         main
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

void gpio_handler(struct k_work *work);
void rotary_handler(struct k_work *work);
K_WORK_DEFINE(gpio_worker, gpio_handler);
K_WORK_DEFINE(rotary_worker, rotary_handler);

struct led_msg led_task = 
{
	.startupAction = 1,
	.poweronAction = 1,
	.advertisingAction = 0,
	.errorAction = 0
};

// struct ble_init_msg ble_init_task =
// {
// 	.init = true
// };

struct advertise_msg advertise_task = 
{
	.adv_config = 1,
	.adv_start = 0,
	.adv_stop = 0,
	.adv_update = 0
};

struct wq_info wq_led_handler1 = {.handle = 1};

ZBUS_CHAN_DEFINE(led_chan,
                struct led_msg,
                NULL,
                NULL,
                ZBUS_OBSERVERS(delay_handler1_lis),
                ZBUS_MSG_INIT(0)
);

ZBUS_CHAN_DEFINE(ble_chan,
                struct advertise_msg,
                NULL,
                NULL,
                ZBUS_OBSERVERS(adv_init_sub),
                ZBUS_MSG_INIT(0)
);

// ZBUS_CHAN_DEFINE(ble_init_chan,
//                 struct ble_init_msg,
//                 NULL,
//                 NULL,
//                 ZBUS_OBSERVERS(ble_init_sub),
//                 ZBUS_MSG_INIT(0)
// );

void timer_1s_handler(struct k_timer *timer_1s)
{
	zbus_chan_pub(&led_chan, &led_task, K_NO_WAIT);
}
K_TIMER_DEFINE(timer_1s, timer_1s_handler, NULL);

void dh1_cb(const struct zbus_channel *chan)
{
	wq_led_handler1.chan = chan;
	k_work_submit(&wq_led_handler1.work);
}
ZBUS_LISTENER_DEFINE(delay_handler1_lis, dh1_cb);

int main(void)
{
	LOG_INF("FT_BLE STARTING UP");

	int ret;

	// Once off task
	k_work_submit(&gpio_worker);
	k_work_submit(&rotary_worker);

	k_work_init(&wq_led_handler1.work, wq_led_cb);

	// ret = zbus_chan_pub(&ble_init_chan, &ble_init_task, K_MSEC(200));
	// if (ret != 0)
	// {
	// 	LOG_ERR("Could not publish to ble init channel");
	// 	return 0;
	// }

	ret = zbus_chan_pub(&led_chan, &led_task, K_MSEC(200));
	if (ret != 0)
	{
		LOG_ERR("Could not publish to led channel");
		return 0;
	}

	// ret = zbus_chan_pub(&ble_chan, &advertise_task, K_MSEC(200));
	// if (ret != 0)
	// {
	// 	LOG_ERR("Could not publish to ble channel");
	// 	return 0;
	// }
	
	k_timer_start(&timer_1s, K_SECONDS(2), K_SECONDS(2));

	led_task.startupAction = 0;
	
	return 0;
}
