/*
* WifiApp.h 
* Create on: 21/2/2025 
* Author: Stephen Truong
*/
#ifndef __WIFIAPP_H__
#define __WIFIAPP_H__
#include <esp_netif.h>

//* Wifi network settings
#define WIFI_AP_SSID               "ESP32_AP"      // Wifi access point name
#define WIFI_AP_PASSWORD           "admin123"      // Wifi access point password
#define WIFI_AP_CHANNEL            1               // Wifi channel 
#define WIFI_AP_SSDI_HIDDEN        0               // Wifi SSID visible 
#define WIFI_AP_MAX_CONNECT        5               // Maximum conneted devices 
#define WIFI_AP_BEACON_INTERVAL    100             // 100ms time lag 
#define WIFI_AP_IP                 "192.169.0.1"   // ESP32's soft IP
#define WIFI_AP_GATEWAY            "192.169.0.1"   // ESP32's IP gateway
#define WIFI_AP_NETMASK            "255.255.255.0" // Access point netmask
#define WIFI_AP_BANDWIDTH          WIFI_BW20       // 20MHz Wifi Bandwidth
#define WIFI_STA_POWER_SAVE        WIFI_PS_NONE    // Disable modem-sleep entirely (decrease latency but higher power consumption) 
#define MAX_SSID_LENGTH            32              // IEEE standard maximum 
#define MAX_PASSWORD_LENGTH        64              // IEEE standard maximum 
#define MAX_CONNECTION_RETRIES     5               // number of retry on disconnect

// Network interface for the station and access point 
extern esp_netif_t *esp_netif_sta;
extern esp_netif_t *esp_netif_ap;

/**
 * Message IDs for Wifi application task
 * @note: Can be expand
 */
typedef enum 
{
    WIFI_APP_MSG_START_HTTP_SERVER = 0,
    WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER,
    WIFI_APP_MSG_STA_CONNECTED_GOT_IP,
}wifi_app_msg_enum;

/** 
 * Struct for message Queue 
 * @note: Can be expand 
 */
typedef struct 
{
    wifi_app_msg_enum msgID;
}wifi_app_queue_msg_t;

/**
 * Sends a message to Queue 
 * @param msgID: message ID from the wifi message enum 
 * @return pdTRUE if an Item successfully sent to the queue, otherwise false
 */
BaseType_t wifi_app_send_msg(wifi_app_msg_enum msgID);

/**
 * Starts the wifi with RTOS task
 */
void wifi_app_start(void);
#endif