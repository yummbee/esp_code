#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_log.h"
#include "esp_system.h"
#include <inttypes.h>

#define DEFAULT_VREF    1100        // Default VREF in mV
#define NO_OF_SAMPLES   64          // Multisampling

static esp_adc_cal_characteristics_t *adc_chars;
static const adc_channel_t channel = ADC_CHANNEL_0;     // GPIO4 if ADC1
static const adc_atten_t atten = ADC_ATTEN_DB_0;
static const adc_unit_t unit = ADC_UNIT_1;

void app_main() {
    // Configure ADC
    if (unit == ADC_UNIT_1) {
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(channel, atten);
    } else {
        adc2_config_channel_atten((adc2_channel_t)channel, atten);
    }

    // Characterize ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);

    // Continuously sample ADC
    while (1) {
        uint32_t adc_reading = 0;
        // Multisampling
        for (int i = 0; i < NO_OF_SAMPLES; i++) {
            if (unit == ADC_UNIT_1) {
                adc_reading += adc1_get_raw((adc1_channel_t)channel);
            } else {
                int raw;
                adc2_get_raw((adc2_channel_t)channel, ADC_WIDTH_BIT_12, &raw);
                adc_reading += raw;
            }
        }
        adc_reading /= NO_OF_SAMPLES;
        // Convert adc_reading to voltage in mV
        uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
        float temperature = (voltage - 500) / 10.0;
       // printf("Raw: %" PRIu32 "\t Voltage: %" PRIu32 " mV\t Temperature: %.2f °C\n", adc_reading, voltage, temperature);
        printf("%.2f\n",temperature);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}