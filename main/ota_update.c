#include "esp_http_client.h"
//#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_system.h"
#include "esp_https_ota.h"
#include "esp_log.h"
#include "ota_update.h"

#define OTA_URL "http://192.168.177.136:8000/mqtt_ota_project_AfterOTA.bin"
static const char *TAG = "OTA";

void start_ota_update(void) {
    esp_http_client_config_t http_config = {
        .url = OTA_URL,
        .timeout_ms = 5000,
        .transport_type = HTTP_TRANSPORT_OVER_TCP,
        .skip_cert_common_name_check = true  // <-- required when no cert is used (plain HTTP)
    };

    esp_https_ota_config_t ota_config = {
        .http_config = &http_config,
    };

    ESP_LOGI(TAG, "Starting OTA from: %s", OTA_URL);

    esp_err_t ret = esp_https_ota(&ota_config);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "OTA successful, restarting...");
        esp_restart();
    } else {
        ESP_LOGE(TAG, "ESP HTTPS OTA Begin failed: %s", esp_err_to_name(ret));
    }
}