// uart.c
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "uart.h"
#include "ota_update.h"

#define TAG "UART_OTA"

void uart_input_task(void *param)
{
    char input_buffer[64];

    while (1) {
        int len = fread(input_buffer, 1, sizeof(input_buffer) - 1, stdin);
        if (len > 0) {
            input_buffer[len] = '\0';  // Null-terminate
            input_buffer[strcspn(input_buffer, "\r\n")] = 0;

            ESP_LOGI(TAG, "Received: %s", input_buffer);

            if (strcmp(input_buffer, "do_ota") == 0) {
                ESP_LOGI(TAG, "Triggering OTA...");
                start_ota_update();
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void uart_start()
{
    xTaskCreate(uart_input_task, "uart_input_task", 4096, NULL, 5, NULL);
}
