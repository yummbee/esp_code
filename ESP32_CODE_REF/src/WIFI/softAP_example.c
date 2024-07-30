#include <stdio.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{

    ESP_LOGE("wifi", "Event dispatched");
    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI("wifi", "Connected to AP successfully");
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGI("wifi", "Disconnected from AP");
            break;
        default:
            break;
        }
    }
    else if (event_base == IP_EVENT)
    {
        switch (event_id)
        {
        case IP_EVENT_STA_GOT_IP:
            ESP_LOGI("ip", "Got IP address");
            // Do something with the IP address
            break;
        default:
            break;
        }
    }
}

static void setup_wifi(void *pvParameters)
{
    ESP_LOGI("main", "Setting up Wi-Fi");
    // This line needs to be called, dont ask me why just does.
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    // Set ESP32 to Access Point (AP) mode
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    // Configure the AP properties
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = "ESP32-Access-Point",
            .ssid_len = strlen("ESP32-Access-Point"),
            .password = "123456789",
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
            .channel = 6,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    // Start Wi-Fi
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI("main", "Wi-Fi set up as AP with DHCP configured");
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
}
