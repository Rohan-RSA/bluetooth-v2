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

#define PRIORITY        7
#define STACK_SIZE      2048
#define SLEEP_TIME      1000

#define LOG_MODULE_NAME         main
LOG_MODULE_REGISTER(LOG_MODULE_NAME);


static void gpio_handler(struct k_work *work);
static K_WORK_DEFINE(gpio_worker, gpio_handler);

int main(void)
{
        LOG_INF("FT_BLE STARTING UP");

        k_work_submit(&gpio_worker);


        return 0;
}


// static void gpio_handler(struct k_work *work)
// {
//         LOG_INF("MY gpio work can be done with this handler function!!!");
// }
