#pragma once

void wq_adv_cb(struct k_work *item);

static struct advertise_msg
{
    int adv_config;
    int adv_start;
    int adv_update;
    int adv_stop;
};



// struct bt_data pto_ad[] =
// {
// 	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),		// If using extended advertising, no flags must be added
// 	//BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_SPS_VAL),						// sd[] data
// 	BT_DATA(BT_DATA_MANUFACTURER_DATA, (unsigned char *)&advertising_packet, sizeof(advertising_packet.advertising_header) + sizeof(advertising_packet.sensor_packet.pto_packet)),
// 	BT_DATA(BT_DATA_NAME_COMPLETE, "FT_PTO", sizeof("FT_PTO")),
// };