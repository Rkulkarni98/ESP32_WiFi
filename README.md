# 🚀 ESP32 OTA Firmware Update with MQTT + UART Trigger – Step-by-Step Guide

Welcome to this beginner-friendly **ESP32 project** that brings together real-time Wi-Fi, MQTT telemetry, UART command listening, and **OTA (Over-the-Air) firmware updates** using ESP-IDF.

We’ll build it piece by piece, and you'll understand **why** each step matters.

---

## 🧠 What We’re Building

> 📡 ESP32 connects to Wi-Fi  
> 🛰️ It publishes uptime to an MQTT broker  
> ⌨️ It listens on UART for a command: `do_ota`  
> 🌐 Upon receiving that command, it downloads a new firmware `.bin` from your PC (hosted via HTTP) and updates itself over Wi-Fi
>
> 
## 🛠️ Step 1: Setup Environment

We’re using **ESP-IDF** (v5+ recommended).  
Follow Espressif’s official setup guide for your OS:  
📘 https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/

Then clone this repo:
```bash
git clone https://github.com/yourusername/ESP32_WiFi.git
cd ESP32_WiFi
```
## 📶 Step 2: Configure Wi-Fi Credentials

To get your ESP32 online, edit:

// In wifi_manager.c
```bash
#define WIFI_SSID "YourWiFiName"
#define WIFI_PASS "YourPassword"
```
✅ This will allow ESP32 to auto-connect and retry on failure.

## 🛰️ Step 3: Add MQTT Publishing

Now that we’re online, we want to push data to the cloud.

We use:
```bash
Public broker: mqtt://broker.hivemq.com

Topic: esp32/test

Payload: ESP32 uptime: XX sec every 15 seconds
```
You'll find this logic in mqtt_client.c.

No authentication is needed for this broker — perfect for testing.

## ⌨️ Step 4: Listen for OTA Command via UART

To let the user trigger OTA updates manually, we use UART input.

We created a FreeRTOS task that monitors serial input (stdin) and looks for:
```bash
do_ota
```
As soon as ESP32 reads that command, it triggers OTA update!

This logic lives in uart.c.

You’ll send do_ota via:

```bash
idf.py monitor
```

## 🌐 Step 5: Prepare for OTA Updates

We want ESP32 to download new firmware over Wi-Fi from a local HTTP server. But first...

📄 What needs to change?
ESP32 must be flashed with a partition table that supports OTA (two apps!)

We must serve the .bin file over HTTP

ESP32 needs the correct OTA URL

## 🧩 Step 6: Set Up partitions.csv

We use a custom partition table:

```bash
# Name,        Type, SubType, Offset,  Size
nvs,           data, nvs,     0x9000,  0x6000
otadata,       data, ota,     0xf000,  0x2000
app0,          app,  ota_0,   0x10000, 0x140000
app1,          app,  ota_1,   ,        0x140000
spiffs,        data, spiffs,  ,        0x190000
```
This lets ESP32 update firmware in app1 while running from app0, and vice versa.

## ⚙️ Step 7: Enable OTA in menuconfig

Run:

```bash
idf.py menuconfig
```

Make these changes:
```bash
Component Config > Partition Table > Custom partition.csv

Component Config > OTA > Enable OTA support

Serial Flasher Config > Flash Size = 4MB
```
Save and exit.

## 🌍 Step 8: Setup the OTA URL

In ota_update.c, update this line:

```bash

#define OTA_URL "http://<your-local-ip>:8000/firmware.bin"
Replace <your-local-ip> with the IP of your PC.
```
Get it using ipconfig (Windows) or ifconfig (Linux/Mac).

## 🔥 Step 9: Flash Initial Firmware

Now we’re ready to build and flash the first OTA-enabled firmware.

```bash
idf.py build
idf.py -p COMx flash     # Replace COMx with your port
idf.py monitor
```

📦 This puts the app into app0 slot.

## 📂 Step 10: Host New Firmware via HTTP Server

After editing code (say, change MQTT payload to AFTER OTA), rebuild:
```bash

idf.py build
```
Copy .bin file to a directory (like Desktop) and run:
```bash

cd ~/Desktop
python3 -m http.server 8000
```
Now ESP32 can download this file over Wi-Fi during OTA!

## 🧪 Step 11: Trigger OTA from UART

While your HTTP server is running and ESP32 is connected to same Wi-Fi...

Open terminal:
```bash
idf.py monitor
```
Type:
```bash
do_ota
```
If all works, you’ll see:

```bash
I (XXXX) OTA: Starting OTA from: http://192.168.X.X:8000/firmware.bin
I (XXXX) OTA: OTA successful, restarting...
```

After reboot, your ESP32 runs the new firmware!

## 🎯 Final Summary
This ESP32 project combines key features needed for real-world IoT development. It connects to a local Wi-Fi network to enable internet access and then starts sending uptime data every 15 seconds to an MQTT broker — a common protocol used in IoT communication. Alongside, the device continuously listens to UART input, and when it receives the command do_ota, it begins the OTA (Over-the-Air) update process. This OTA update fetches a new firmware .bin file hosted on a local HTTP server running on your PC. To safely switch between the current firmware and the new one, the project uses a custom partition table that supports dual OTA slots (app0 and app1). This setup makes it possible to update your ESP32 remotely and reliably, which is a major advantage in any smart product or connected device.

## 📌 Useful Commands
```bash
idf.py build            # Build project
idf.py -p COMx flash    # Flash firmware
idf.py monitor          # Open serial monitor
idf.py menuconfig       # Configure project settings
python3 -m http.server  # Serve OTA firmware
```




