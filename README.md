# 👓 WiFi Smart Camera Glasses

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A wearable FPV (First Person View) camera system built using the **ESP32-CAM** module. This project allows for real-time video streaming over a local WiFi network, providing a POV perspective directly to your browser.

![Front View](images/wifi%20cam%20glasses%20front%20look.jpeg)

---

## 🚀 Project Overview

The **WiFi Smart Camera Glasses** is an open-source hardware project focused on creating a portable, battery-powered streaming device. It captures live video using the OV2640 camera and serves it via an MJPEG stream over HTTP.

### Key Features
- **Live Streaming:** Real-time MJPEG video via local IP.
- **Ultra-Portable:** Lightweight design powered by a Li-Po battery.
- **Integrated Charging:** Built-in TP4056 charging module with micro-USB.
- **Adjustable Power:** XL6009 Boost Converter ensures stable 5V output for the ESP32-CAM.
- **POV Perspective:** Wearable form factor for hands-free recording/streaming.

---

## 🛠️ Components List

| Component | Description |
| :--- | :--- |
| **ESP32-CAM** | Main microcontroller with WiFi and Camera support |
| **OV2640** | 2MP Camera module |
| **TP4056** | Li-Po Battery charging module (with protection) |
| **XL6009** | DC-DC Boost Converter (Step-up to 5V) |
| **Li-Po Battery** | 500mAh 3.7V rechargeable battery |
| **Slide Switch** | Miniature power control |
| **Glasses Frame** | Base for mounting the components |

---

## 🔌 Circuit Diagram

The circuit ensures stable power delivery and easy charging.

![Circuit Diagram](circuit/circuit%20design.jpeg)

### Power Path:
1. **Battery** → **TP4056** (Charging)
2. **TP4056 Output** → **Slide Switch**
3. **Slide Switch** → **XL6009 Boost Converter** (Adjusted to 5V)
4. **XL6009 Output** → **ESP32-CAM 5V Pin**

---

## 📸 Gallery

<p align="center">
  <img src="images/wifi%20cam%20glasses%20side%20look.jpeg" width="45%" alt="Side View" />
  <img src="images/wifi%20cam%20glasses%20front%20look.jpeg" width="45%" alt="Front View" />
</p>

---

## ⚙️ Setup & Installation

### 1. Hardware Assembly
- Connect the components as shown in the [Circuit Diagram](#-circuit-diagram).
- **CRITICAL:** Use a multimeter to adjust the **XL6009** output to exactly **5V** before connecting it to the ESP32-CAM to avoid damage.

### 2. Software Configuration
- Open `code/esp32_cam_glasses.ino` in the **Arduino IDE**.
- Install the **ESP32** board support via the Boards Manager.
- Update your WiFi credentials:
  ```cpp
  const char* sta_ssid = "YOUR_WIFI_SSID";
  const char* sta_password = "YOUR_WIFI_PASSWORD";
  ```

### 3. Flashing
- Connect an **FTDI Adapter** to the ESP32-CAM.
- Bridge **GPIO 0** to **GND** to enter Flash Mode.
- Select Board: **"AI Thinker ESP32-CAM"**.
- Click **Upload**.

### 4. Accessing the Stream
- Remove the GPIO 0 bridge and reset the device.
- Open the Serial Monitor (115200 baud).
- Copy the IP address and visit `http://[IP-ADDRESS]/stream` in your browser.

---

## 📜 License

This project is licensed under the **MIT License**. See the [LICENSE](LICENSE) file for the full text.

Developed by **Vishak P**