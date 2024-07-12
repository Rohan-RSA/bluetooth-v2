#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/hci.h>

#include "advertise_task.h"
#include "led_task/led_task.h"

#define LOG_MODULE_NAME advertise_task
#define ADVERTISING_START_WORD                  0x5321    //FT
#define ADVERTISING_COMPANY_IDENTIFIER          0x4654    //S!
#define DEVICE_NAME BT_DATA_NAME_COMPLETE

LOG_MODULE_REGISTER(LOG_MODULE_NAME);

ZBUS_CHAN_DECLARE(ble_chan);
ZBUS_CHAN_DECLARE(led_chan);

char *advertising_friendly_name[] = {"FT_PTO", "FT_PRESSURE","FT_FLOW"};
char *advertising_sensor_type[] = {0x0001, 0x0002, 0x0003};

typedef struct _sensor_select_type_t
{
	bool pressure;
	bool pto;
	bool flow;
}sensor_select_type_t;

sensor_select_type_t selection;

typedef struct _pto_packet_t
{
  int32_t   pto_left_io;
  int32_t   pto_right_io;
}pto_packet_t ;

typedef struct _pressure_packet_t
{
  int32_t   pressure_left_value;
  int32_t   pressure_right_value;
}pressure_packet_t ;

typedef struct _flow_packet_t
{
  int32_t   flow_left_value;
  int32_t   flow_rigth_value;
}flow_packet_t ;

typedef union _sensor_packet_u
{
	pressure_packet_t	pressure_packet;
	flow_packet_t		flow_packet;
	pto_packet_t		pto_packet;
}sensor_packet_u ;

typedef struct _advertising_header_t
{
	uint16_t company_identifier;
  	uint16_t advertising_start_word;
  	uint16_t advertising_sensor_type;

}advertising_header_t ;

typedef struct _advertising_packet_t 
{
	advertising_header_t 	advertising_header;
  	sensor_packet_u    		sensor_packet;

}advertising_packet_t ; 

advertising_packet_t advertising_packet  = 
{
	.advertising_header.company_identifier = ADVERTISING_COMPANY_IDENTIFIER,
  	.advertising_header.advertising_start_word = ADVERTISING_START_WORD,
  	.advertising_header.advertising_sensor_type = NULL,
	// .advertising_header.advertising_sensor_type = ADVERTISING_SENSOR_TYPE,
};

struct bt_le_ext_adv *ft_adv;

struct bt_data pto_ad[] =
{
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),		// If using extended advertising, no flags must be added
	//BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_SPS_VAL),						// sd[] data
	BT_DATA(BT_DATA_MANUFACTURER_DATA, (unsigned char *)&advertising_packet, sizeof(advertising_packet.advertising_header) + sizeof(advertising_packet.sensor_packet.pto_packet)),
	BT_DATA(BT_DATA_NAME_COMPLETE, "FT_PTO", sizeof("FT_PTO")),
};

ZBUS_SUBSCRIBER_DEFINE(adv_init_sub, 4);

static void adv_init_task(void)
{
  const struct zbus_channel *chan;

  while (!zbus_sub_wait(&adv_init_sub, &chan, K_FOREVER))
  {
    int ret;

    struct advertise_msg adv_msg;
    struct advertise_sensor_type sensor_type;

    zbus_chan_read(chan, &adv_msg, K_MSEC(200));
    zbus_chan_read(chan, &sensor_type, K_MSEC(200));

		LOG_INF("Advertise msg processed by THREAD handler adv_init_sub:\r\nconfig advertising = %d,\r\nstart advertising = %d,\r\nupdate advertising = %d,\r\nstop advertising = %d",
              adv_msg.adv_config, adv_msg.adv_start, adv_msg.adv_update, adv_msg.adv_stop);

    LOG_INF("Sensor type processed by THREAD handler adv_init_sub:\r\npto = %d,\r\npressure = %d,\r\nflow = %d",
              sensor_type.pto, sensor_type.pressure, sensor_type.flow);

    advertising_packet.advertising_header.advertising_sensor_type = 0x002;
    const struct bt_le_adv_param ft_params = BT_LE_ADV_PARAM_INIT(BT_LE_ADV_OPT_USE_IDENTITY |
                                                                  BT_LE_ADV_OPT_EXT_ADV | BT_LE_ADV_OPT_CODED,
                                                                  BT_GAP_ADV_SLOW_INT_MIN,
                                                                  BT_GAP_ADV_SLOW_INT_MAX,
                                                                  NULL);
    
    if (adv_msg.adv_config == 1)
    {
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

        ret = bt_le_ext_adv_set_data(ft_adv, pto_ad, ARRAY_SIZE(pto_ad), NULL, 0);
        if (ret !=0 )
        {
          LOG_ERR("Failed to set advertiser data (err %d)", ret);
          return ret;
        }
        LOG_INF("Succesfully set advertising data %p for set %p",  pto_ad ,(void*) ft_adv);  

        ret = bt_le_ext_adv_start(ft_adv, NULL);
        if (ret != 0)
        {
          LOG_ERR("Failed to start advertising set %p with error code %d", (void*) ft_adv, ret);
        }
        LOG_INF("Succesfully started advertising set %p", (void*) ft_adv);      
      }
    }
    


  }
}
K_THREAD_DEFINE(adv_init_id, 1024, adv_init_task, NULL, NULL, NULL, 3, 0, 0);

// int bt_ready(int ret)
// {


// }

// void wq_adv_cb(struct k_work *item)
// {
//   int ret;
//   const struct advertise_msg msg;
//   struct wq_info *adv = CONTAINER_OF(item, struct wq_info, work);

//   static struct led_msg;
//   static struct led_msg led_task = 
//   {
//       .startupAction = 0,
//       .poweronAction = 0,
//       .advertisingAction = 0,
//       .errorAction = 0
//   };

//   zbus_chan_read(adv->chan, &msg, K_MSEC(200));

//   LOG_INF("Advertising msg processed by WORK QUEUE handler wq_adv_cb with handle %u: config advertising = %d, start advertising = %d, update advertising = %d, stop advertising = %d",
//   adv->handle, msg.adv_config, msg.adv_start, msg.adv_update, msg.adv_stop);


  // if (msg.adv_config)
  // {
  //   if (bt_is_ready)
  //   {
  //     // k_work_submit_to_queue(&adv_work_q, );
  //     ret = bt_enable(NULL);
  //     if (ret != 0)
  //     {
  //       LOG_ERR("Bluetooth init failed (err %d)", ret);

  //       led_task.errorAction = 1;
    
  //       ret = zbus_chan_pub(&led_chan, &led_task, K_MSEC(200));
  //       if (ret != 0)
  //       {
  //           LOG_ERR("Could not publish error led task to ble channel");
  //           return 0;
  //       }
  //       LOG_INF("Published led error task to ble channel");
  //       return 0;
  //     }
  //     LOG_INF("BLE init completed.");

  //     k_msleep(10000);

  //     ret = bt_le_ext_adv_create(&ft_params, NULL, &ft_adv);
  //     if (ret)
  //     {
  //       LOG_ERR("Failed to create advertiser set (err %d)", ret);
  //       return ret;
  //     }
  //     LOG_INF("Created extended advertising set ft_adv: %p", (void*) ft_adv);
  //   }

  // }
  // led_task.advertisingAction = 1;
  // ret = zbus_chan_pub(&led_chan, &led_task, K_MSEC(200));
  // if (ret != 0)
  // {
  //   LOG_ERR("Could not publish to ble channel");
  //   return 0;
  // }
// };




