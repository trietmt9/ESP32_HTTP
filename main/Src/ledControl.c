/*
* ledControl.c
* Create on: 21/2/2025 
* Author: Stephen Truong
*/

#include <ledControl.h>
/**
 * @brief: Blinking LED during initialize state
 */
void led_Wifi_Init(void)
{
    gpio_set_direction(ON_BOARD_LED, GPIO_MODE_OUTPUT);
    gpio_set_level(ON_BOARD_LED, HIGH);
    vTaskDelay(LED_DELAY(3));
    gpio_set_level(ON_BOARD_LED, LOW);
    vTaskDelay(LED_DELAY(3));
}

/**
 * @brief: Blinking LED during starting http server 
 */
void led_http_started(void)
{
    gpio_set_level(ON_BOARD_LED, HIGH);
    vTaskDelay(LED_DELAY(10));
    gpio_set_level(ON_BOARD_LED, LOW);
    vTaskDelay(LED_DELAY(10));
}
/**
 * @brief: Led Light Wifi connected
 */
void led_wifi_connected(void)
{
    gpio_set_level(ON_BOARD_LED, HIGH);
    vTaskDelay(LED_DELAY(10));
    gpio_set_level(ON_BOARD_LED, LOW);
    vTaskDelay(LED_DELAY(10));
    gpio_set_level(ON_BOARD_LED, HIGH);
    vTaskDelay(LED_DELAY(10));
    gpio_set_level(ON_BOARD_LED, LOW);
    vTaskDelay(LED_DELAY(10));
}