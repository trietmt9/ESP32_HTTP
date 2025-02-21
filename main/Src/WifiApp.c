/*
* WifiApp.c
* Create on: 21/2/2025 
* Author: Stephen Truong
*/
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/task.h>

#include <esp_err.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <lwip/netdb.h>

#include <TasksCommon.h>
#include <WifiApp.h>
#include <ledControl.h>

// Tag used for ESP console log message
static const char TAG[] = "Wifi App"; 

// Queue handle used to manipulate the main queue of event 
static QueueHandle_t wifi_app_queue_handle;

// netif objects for station and access point 
esp_netif_t* esp_netif_ap  = NULL;
esp_netif_t* esp_netif_sta = NULL;

/**
 * @brief: Wifi event handler for wifi and ip event 
 * @param: arg: data aside from event data, that is passed to the handler when it is called 
 * @param: event_base: the base id of the event to register the handler for 
 * @param: event_id: the id of the event to register the handler for
 * @param: event_data: the data of the event 
 */
static void wifi_app_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if(event_base == WIFI_EVENT)
    {
        switch(event_id)
        {
            case WIFI_EVENT_AP_START:
                ESP_LOGI(TAG, "WIFI_EVENT_AP_START");
                break;

            case WIFI_EVENT_AP_STOP:
                ESP_LOGI(TAG, "WIFI_EVENT_AP_STOP");
                break;

            case WIFI_EVENT_AP_STACONNECTED:
                ESP_LOGI(TAG, "WIFI_EVENT_AP_STACONNECTED");
                break;

            case WIFI_EVENT_AP_STADISCONNECTED:
                ESP_LOGI(TAG, "WIFI_EVENT_AP_STADISCONNECTED");
                break;

            case WIFI_EVENT_STA_START:
                ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
                break;

            case WIFI_EVENT_STA_CONNECTED:
                ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED");
                break;

            case WIFI_EVENT_STA_DISCONNECTED:
                ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");
                break;

            default:
                break;
        }
    }
    else if(event_base == IP_EVENT)
    {
        switch(event_id)
        {
            case IP_EVENT_STA_GOT_IP:
                ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");
                break;
            default:
                break;
        }
    }
}

/**
 * @brief: Initialize the TCP stack and default Wifi configuration 
 */
static void wifi_app_default_wifi_init()
{
    // Initialize the TCP stack 
    ESP_ERROR_CHECK(esp_netif_init());

    // Default Wifi configuration
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    esp_netif_sta = esp_netif_create_default_wifi_sta();
    esp_netif_ap = esp_netif_create_default_wifi_ap();
}

/**
 * @brief: Configure Wifi access point settings and assigs the static IP to the softAP 
 */
static void wifi_app_soft_ap_config(void)
{
    // SoftAP - Wifi access point configuration 
    wifi_config_t ap_config ={
        .ap = {
            .ssid            = WIFI_AP_SSID,
            .ssid_len        = strlen(WIFI_AP_SSID),
            .password        = WIFI_AP_PASSWORD,
            .channel         = WIFI_AP_CHANNEL,
            .ssid_hidden     = WIFI_AP_SSDI_HIDDEN,
            .authmode        = WIFI_AUTH_WPA2_PSK,
            .max_connection  = WIFI_AP_MAX_CONNECT,
            .beacon_interval = WIFI_AP_BEACON_INTERVAL,
        }
    };

    // Configure DHCP for AP 
    esp_netif_ip_info_t ap_ip_info;
    memset(&ap_ip_info, 0, sizeof(ap_ip_info));
    esp_netif_dhcps_stop(esp_netif_ap);
    inet_pton(AF_INET, WIFI_AP_IP, &ap_ip_info.ip); // assign access point static IP, GW and Net Mask
    inet_pton(AF_INET, WIFI_AP_GATEWAY, &ap_ip_info.gw);
    inet_pton(AF_INET, WIFI_AP_NETMASK, &ap_ip_info.netmask);
    ESP_ERROR_CHECK(esp_netif_set_ip_info(esp_netif_ap, &ap_ip_info));  // Statically configure the network interface 
    ESP_ERROR_CHECK(esp_netif_dhcps_start(esp_netif_ap));               // Start DHCP server

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));                // Set the wifi mode to AP and STA
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config));   // Set the wifi configuration for the access point
    ESP_ERROR_CHECK(esp_wifi_set_bandwidth(WIFI_IF_AP, WIFI_AP_BANDWIDTH)); // Set the wifi bandwidth for the access point
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_STA_POWER_SAVE)); // Set the power save mode for the station


}
/**
 * @brief: Initialize the wifi application event handler for wifi and ip event
 */
static void wifi_app_event_handler_init(void)
{
    // Event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Event handler for connection
    esp_event_handler_instance_t instance_wifi_event;
    esp_event_handler_instance_t instance_ip_event;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_app_event_handler, NULL, &instance_wifi_event));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, ESP_EVENT_ANY_ID, &wifi_app_event_handler, NULL, &instance_ip_event));
}

/**
 * @brief: Main task for the wifi application 
 */
static void wifi_app_task(void *pvParameter)
{
    wifi_app_queue_msg_t msg;

    // Initialize event handler 
    wifi_app_event_handler_init();

    // Initialize the TCP/IP stack and Wifi config 
    wifi_app_default_wifi_init();

    // SoftAP Config
    wifi_app_soft_ap_config();

    // Start wifi
    ESP_ERROR_CHECK(esp_wifi_start());

    // Send first event message
    wifi_app_send_msg(WIFI_APP_MSG_START_HTTP_SERVER);

    while(1)
    {
        if(xQueueReceive(wifi_app_queue_handle, &msg, portMAX_DELAY))
        {
            switch(msg.msgID)
            {
                case WIFI_APP_MSG_START_HTTP_SERVER:
                    ESP_LOGI(TAG, "WIFI_APP_MSG_START_HTTP_SERVER");
                    //http_server_start();
                    led_http_started();
                    break;
                case WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER:
                    ESP_LOGI(TAG, "WIFI_APP_MSG_START_HTTP_SERVER");
                    break;
                case WIFI_APP_MSG_STA_CONNECTED_GOT_IP:
                    ESP_LOGI(TAG, "WIFI_APP_MSG_STA_CONNECTED_GOT_IP");
                    led_wifi_connected();
                    break;
                default:
                    break;
            }
        }
    }
}

BaseType_t wifi_app_send_msg(wifi_app_msg_enum msgID)
{
    wifi_app_queue_msg_t msg;
    msg.msgID = msgID;
    return xQueueSend(wifi_app_queue_handle, &msg, portMAX_DELAY); 
}

/**
 * @brief: Initialize wifi task and wifi  
 */
void wifi_app_start(void)
{   
    ESP_LOGI(TAG, "Starting wifi application");
    // Start wifi led 
    led_Wifi_Init();

    // Disable default Wifi Logging messeage 
    esp_log_level_set("Wifi", ESP_LOG_NONE);

    // Create message queue 
    wifi_app_queue_handle = xQueueCreate(3, sizeof(wifi_app_queue_msg_t));

    // Start wifi application task
    xTaskCreatePinnedToCore(&wifi_app_task,"wifi_app_task", WIFI_AP_TASK_STACKSIZE, NULL, WIFI_AP_TASK_PRIORITY, NULL, WIFI_AP_TASK_COREID);

}