#pragma once

void wq_adv_cb(struct k_work *item);

static struct advertise_msg
{
    int adv_config;
    int adv_start;
    int adv_update;
    int adv_stop;
};


