#include <stdio.h>
#include "mqtt_client.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

static const char *TAG = "MQTT";
static esp_mqtt_client_handle_t mqtt_client = NULL;

// Task to publish message every 5 sec
void mqtt_publish_task(void *pvParameters)
{
    while (1) {
        if (mqtt_client != NULL) {
            char payload[64];
            sprintf(payload, "posting payload ESP32 uptime: %lld sec", (long long)esp_timer_get_time() / 1000000);
            int msg_id = esp_mqtt_client_publish(mqtt_client, "esp32/test", payload, 0, 1, 0);
            ESP_LOGI("MQTT", "Published message Before OTA ID %d: %s", msg_id, payload);
        }

        vTaskDelay(pdMS_TO_TICKS(15000));  // Delay 5 sec
    }
}


// Correct event handler for ESP-IDF v5
static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                               int32_t event_id, void *event_data)
{
    // esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT connected ✅");

            //     int msg_id = esp_mqtt_client_publish(mqtt_client, "esp32/test/coding", "Hello from ESP32!!@!!!!!!!!!", 0, 1, 0);
            // ESP_LOGI(TAG, "Published message with ID: %d", msg_id);
             // Start the task that publishes every 5 seconds
            xTaskCreate(&mqtt_publish_task, "mqtt_pub_task", 4096, NULL, 5, NULL);
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "MQTT disconnected ❌");
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT Error!");
            break;

        default:
            break;
    }
}

void mqtt_app_start(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://broker.hivemq.com",  // ✅ correct for ESP-IDF v5
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}
