# custom ws2811ic-wifi-Digital-Clock

A WiFi-enabled 4-digit RGB segmented clock built using **WS2811 addressable LEDs** and an **ESP8266**.
This project is designed as an experimental **desk display platform** for learning embedded systems, LEDs, and IoT concepts.

---

## 🔧 Features
- Automatic time synchronization using NTP
- WiFi auto-connect with **AP fallback mode**
- RGB segmented display using WS2811 ICs
- Colon LED used as **status + seconds indicator**
- 12-hour time format
- Modular design for future upgrades

---

## 📷 Prototype Images

---

## 🧠 How It Works
- ESP8266 connects to saved WiFi credentials
- If WiFi is not available, device enters **AP configuration mode**
- Time is fetched from an NTP server
- Each digit segment is mapped to WS2811 RGB channels
- Colon LEDs blink every second and show system status

---

## 🧩 Hardware Used
- ESP8266 (NodeMCU)
- WS2811 RGB LED ICs
- 5V Power Supply
- SMD LEDs (white)
- Prototype PCB / Breadboard

> ♻️ Some components used in the prototype were salvaged from old PCBs and tested before use.

---

## 📂 Folder Structure
---

