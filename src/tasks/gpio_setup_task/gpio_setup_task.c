#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include "gpio_setup_task.h"

#define LOG_MODULE_NAME         gpio_setup_task

LOG_MODULE_REGISTER(LOG_MODULE_NAME);

const struct gpio_dt_spec power_led     = GPIO_DT_SPEC_GET(POWER_LED, gpios);
const struct gpio_dt_spec rx_led        = GPIO_DT_SPEC_GET(RX_LED, gpios);
const struct gpio_dt_spec tx_led        = GPIO_DT_SPEC_GET(TX_LED, gpios);
// const struct gpio_dt_spec ble_con_led   = GPIO_DT_SPEC_GET(BLE_CON_LED, gpios);
// const struct gpio_dt_spec ble_led       = GPIO_DT_SPEC_GET(BLE_LED, gpios);


void gpio_handler(struct k_work *work)
{
    int ret;

    LOG_INF("Initializng GPIO");

    ret = gpio_pin_configure_dt(&power_led, GPIO_OUTPUT);
    if (ret != 0)
    {
        LOG_ERR("Could not configure the power LED");
    }  
    ret = gpio_pin_configure_dt(&rx_led, GPIO_OUTPUT);
    if (ret != 0)
    {
        LOG_ERR("Could not configure the rx LED");
    }  
    ret = gpio_pin_configure_dt(&tx_led, GPIO_OUTPUT);
    if (ret != 0)
    {
        LOG_ERR("Could not configure the tx LED");
    }  
    // ret = gpio_pin_configure_dt(&ble_con_led, GPIO_OUTPUT);
    //     if (ret != 0)
    // {
    //     LOG_ERR("Could not configure the ble connection LED");
    // }  
    // ret = gpio_pin_configure_dt(&ble_led, GPIO_OUTPUT);
    // if (ret != 0)
    // {
    //     LOG_ERR("Could not configure the ble status LED");
    // }   
    
}








