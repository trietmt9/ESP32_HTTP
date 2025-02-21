/*
* ledControl.h
* Create on: 21/2/2025 
* Author: Stephen Truong
*/
#ifndef __LEDCONTROL_H__
#define __LEDCONTROL_H__

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>

#define LED_DELAY(X)            X/portTICK_PERIOD_MS
#define ON_BOARD_LED            GPIO_NUM_18
typedef enum
{
    LOW = 0,
    HIGH
}ledstate_enum;
/**
 * @brief: Blinking LED during initialize state
 */
void led_Wifi_Init(void);

/**
 * @brief: Blinking LED during starting http server 
 */
void led_http_started(void);

/**
 * @brief: Led Light Wifi connected
 */
void led_wifi_connected(void);

#endif