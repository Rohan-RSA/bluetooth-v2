#pragma once

#include <zephyr/kernel.h>

enum led_action {
    STARTUP = 1,
    POWERON = 2,
    ERROR = 4,
    ADVERTISING = 8,
    CONNECTED = 16,
    DISCONNECTED = 32,
};

struct led_work_s {

    struct k_work work;
    // Node UUID's
    enum led_action led_action;
};

void led_handler(struct k_work *work);
