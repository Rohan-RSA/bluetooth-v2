#pragma once

#include <zephyr/kernel.h>

struct wq_info {
	struct k_work work;
	const struct zbus_channel *chan;
	uint8_t handle;
};

struct led_msg {
    int startupAction;
    int poweronAction;
    int advertisingAction;
    int errorAction;
};

void wq_led_cb(struct k_work *item);

// struct wq_info wq_led_handler1;
// void led_handler(struct k_work *work);
