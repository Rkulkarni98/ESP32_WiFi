# Add the source files for the project
# This is the main CMakeLists.txt file for the ESP-IDF project
idf_component_register(SRCS
    "wifi_manager.c"
    "mqtt_client.c"
    "ota_update.c"
    "uart.c"
    INCLUDE_DIRS ".")
