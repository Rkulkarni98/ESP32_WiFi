#ifndef OTA_UPDATE_H
#define OTA_UPDATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"

// Function to start OTA update from a given URL
void start_ota_update(void);

#ifdef __cplusplus
}
#endif

#endif // OTA_UPDATE_H
