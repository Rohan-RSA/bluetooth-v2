#pragma once

#include <zephyr/kernel.h>

// enum led_action {
//     STARTUP = 1,
//     POWERON = 2,
//     ERROR = 4,
//     ADVERTISING = 8,
//     CONNECTED = 16,
//     DISCONNECTED = 32,
// };

// struct led_work_s {

//     struct k_work work;
//     // Node UUID's
//     enum led_action led_action;
// };

struct wq_info {
	struct k_work work;
	const struct zbus_channel *chan;
	uint8_t handle;
};

static struct wq_info wq_led_handler1 = {.handle = 1};

struct led_msg{
    int startupAction;
    int poweronAction;
    int advertisingAction;
    int errorAction;
};

// void led_handler(struct k_work *work);
