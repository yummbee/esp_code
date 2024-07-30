// This is an example of an esp32 hosted webserver and webpage that can be used to control the onboard LED.
#include "esp_log.h"
#include "esp_err.h"
#include "esp_http_server.h"
#include "driver/gpio.h"

#define LED_GPIO 2

bool LED_STATE = 0;
// This method serves the HTML page of the /hello uri
esp_err_t hello_get_handler(httpd_req_t *req)
{
    const char *html_resp = "<!DOCTYPE html>"
                            "<html>"
                            "<head><title>ESP32 LED Control</title></head>"
                            "<body>"
                            "<h2>ESP32 LED Control</h2>"
                            "<button onclick=\"toggleLED()\">Toggle LED</button>"
                            "<script>"
                            "function toggleLED() {"
                            "  var xhttp = new XMLHttpRequest();"
                            "  xhttp.open('GET', '/toggle', true);"
                            "  xhttp.send();"
                            "}"
                            "</script>"
                            "</body>"
                            "</html>";
    httpd_resp_send(req, html_resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t toggle_led_handler(httpd_req_t *req)
{
    // int current_state = gpio_get_level(LED_GPIO);
    ESP_LOGI("LED INFO", "toggling led");
    if (LED_STATE == 0)
    {
        gpio_set_level(LED_GPIO, 1);
        LED_STATE = 1;
        ESP_LOGI("LED INFO", "turning led on");
    }
    else
    {
        ESP_LOGI("LED INFO", "turning led off");
        LED_STATE = 0;
        gpio_set_level(LED_GPIO, 0);
    }

    // gpio_set_level(LED_GPIO, !current_state);
    httpd_resp_send(req, NULL, 0); // Send an empty response to acknowledge
    return ESP_OK;
}

httpd_uri_t toggle = {
    .uri = "/toggle",
    .method = HTTP_GET,
    .handler = toggle_led_handler,
    .user_ctx = NULL};

httpd_uri_t hello = {
    .uri = "/hello",
    .method = HTTP_GET,
    .handler = hello_get_handler,
    .user_ctx = NULL};

void start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    if (httpd_start(&server, &config) == ESP_OK)
    {
        // if the server starts correctly, register the URI handlers
        httpd_register_uri_handler(server, &hello);
        httpd_register_uri_handler(server, &toggle);
    }
}