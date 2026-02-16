# ENGR 4321: ESP32 Wi-Fi Connectivity & Landing Page Lab

## Overview

This lab establishes a local "Command Center" using a Python Flask server. The instructor's computer acts as the host, and student ESP32 devices connect via the class Spectrum router to register their presence and send status updates.

**Security Features:**

* **User-Agent Filtering:** Only devices identifying as `ESP32-Class-Device` are accepted.
* **Auth Token:** A shared class token prevents unauthorized connections.
* **Input Sanitization:** Incoming data is escaped to prevent HTML injection on the dashboard.
* **Secrets Management:** Sensitive credentials are stored in `.env` and `secrets.h` files, keeping them out of source control.

---

## Part 1: Host Setup (Instructor)

### 1. Prerequisites

Ensure you have Python installed. Install Flask and the DotEnv library:

```bash
pip install flask python-dotenv
```

### 2. Secrets Configuration (`.env`)

To keep the class token secure when uploading to GitHub, we use environment variables.

1.  Create a file named `.env` in the same folder as your server script.
2.  Add the following content:
    ```ini
    CLASS_TOKEN=ENGR4321_Spring2026_Secure
    FLASK_ENV=development
    ```
3.  **Git Safety:** Create a `.gitignore` file and add `.env` to it so your secrets are never uploaded.
4.  *Optional:* Create a `.env.example` file with dummy data to upload to GitHub as a template.

### 3. Server Code (`server.py`)

Save the following code as `server.py`. It automatically loads the token from your `.env` file.

```python
import os
from flask import Flask, request, jsonify, render_template_string
import datetime
import html
from dotenv import load_dotenv

# Load environment variables from .env file
load_dotenv()

app = Flask(__name__)

# --- CONFIGURATION ---
CLASS_TOKEN = os.getenv("CLASS_TOKEN")
REQUIRED_USER_AGENT = "ESP32-Class-Device"

# Store connected devices in memory {mac_address: {data}}
connected_devices = {}

# HTML Dashboard Template
HTML_TEMPLATE = """
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Class Hub</title>
    <meta http-equiv="refresh" content="2">
    <style>
        body { font-family: 'Courier New', monospace; background-color: #1a1a1a; color: #00ff00; padding: 20px; }
        h1 { border-bottom: 2px solid #00ff00; padding-bottom: 10px; }
        .device-card { border: 1px solid #00ff00; margin: 10px 0; padding: 10px; border-radius: 5px; background: #222; }
        .device-header { font-weight: bold; font-size: 1.2em; }
        .timestamp { color: #888; font-size: 0.8em; }
        .message { color: #fff; margin-top: 5px; }
    </style>
</head>
<body>
    <h1>> SYSTEM MONITOR: ESP32 CONNECTIONS</h1>
    <div id="device-list">
        {% if devices %}
            {% for mac, data in devices.items() %}
            <div class="device-card">
                <div class="device-header">ID: {{ data.id }} <span style="font-size:0.8em">({{ mac }})</span></div>
                <div class="message">Input: "{{ data.message }}"</div>
                <div class="timestamp">Last Seen: {{ data.last_seen }}</div>
            </div>
            {% endfor %}
        {% else %}
            <p>Waiting for devices...</p>
        {% endif %}
    </div>
</body>
</html>
"""

def verify_request(req):
    """Verify User-Agent and Token."""
    agent = req.headers.get('User-Agent', '')
    token = req.headers.get('X-Auth-Token', '')
    
    # Fail if token is missing from .env or doesn't match
    if not CLASS_TOKEN:
        print("[-] Error: CLASS_TOKEN not set in .env file")
        return False
        
    return agent == REQUIRED_USER_AGENT and token == CLASS_TOKEN

@app.route('/')
def index():
    """Dashboard view."""
    sorted_devices = dict(sorted(connected_devices.items(), key=lambda item: item[1]['last_seen'], reverse=True))
    return render_template_string(HTML_TEMPLATE, devices=sorted_devices)

@app.route('/connect', methods=['POST'])
def connect():
    """Endpoint for ESP32 devices."""
    if not verify_request(request):
        print(f"[-] Unauthorized access attempt from {request.remote_addr}")
        return jsonify({"status": "denied", "reason": "Security Check Failed"}), 403

    try:
        data = request.json
        if not data:
            return jsonify({"status": "error", "reason": "No JSON data"}), 400

        # Sanitize inputs
        device_id = html.escape(data.get("id", "Unknown"))
        mac_addr = html.escape(data.get("mac", "Unknown"))
        message = html.escape(data.get("msg", "No Data"))

        connected_devices[mac_addr] = {
            "id": device_id,
            "message": message,
            "last_seen": datetime.datetime.now().strftime("%H:%M:%S")
        }

        print(f"[+] Device Connected: {device_id} ({mac_addr})")
        return jsonify({"status": "accepted"}), 200

    except Exception as e:
        print(f"[-] Error: {e}")
        return jsonify({"status": "error"}), 500

if __name__ == '__main__':
    # Host 0.0.0.0 allows access from other devices on the network
    app.run(host='0.0.0.0', port=8080, debug=True)
```

## Part 2: Client Setup (Students)

### 1. Requirements

* **Hardware:** ESP32 Development Board
* **Software:** Arduino IDE with ESP32 board support installed.

### 2. Client Secrets (`secrets.h`)

Instead of hardcoding passwords, create a new tab in the Arduino IDE named `secrets.h` and paste the following. **Do not upload this file to GitHub.**

```cpp
#ifndef SECRETS_H
#define SECRETS_H

#define WIFI_SSID "SpectrumSetup-XX"       // WIFI NAME
#define WIFI_PASS "router_password"        // WIFI PASSWORD
#define CLASS_TOKEN "ENGR4321_Spring2026_Secure" // MUST MATCH SERVER .ENV

#endif
```

### 3. Client Code (`ESP32_WiFi_Lab.ino`)

Copy the code below. You must modify `serverIP` and `studentName`.

```cpp
#include <WiFi.h>
#include <HTTPClient.h>
#include "secrets.h" // Import credentials

// --- CONFIGURATION REQUIRED ---
const char* serverIP = "192.168.1.XXX";     // INSTRUCTOR IP (See Whiteboard)
const char* studentName = "Group_1_Red";    // YOUR GROUP NAME

// --- SECURITY (From secrets.h) ---
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;
const char* classToken = CLASS_TOKEN;
const char* userAgent = "ESP32-Class-Device";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Network!");8
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Construct URL: http://<IP>:8080/connect for example (note that port 5000 is decent however you may run into issues with features like AirDrop)
    String serverPath = "http://" + String(serverIP) + ":8080/connect";
    
    http.begin(serverPath);
    
    // Add Security Headers
    http.addHeader("Content-Type", "application/json");
    http.addHeader("User-Agent", userAgent);
    http.addHeader("X-Auth-Token", classToken);

    // Create JSON Payload
    String mac = WiFi.macAddress();
    String msg = "System Ready. Uptime: " + String(millis()/1000) + "s";
    
    String jsonPayload = "{\"id\":\"" + String(studentName) + "\", \"mac\":\"" + mac + "\", \"msg\":\"" + msg + "\"}";

    // Send POST Request
    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Server Response: " + response);
    } else {
      Serial.print("Error sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }

  // Send update every 5 seconds
  delay(5000);
}
```

## Part 3: Running the Lab

### 1. Instructor: Get Local IP

Open Command Prompt (Windows) or Terminal (Mac/Linux) to find the local IP address assigned by the Spectrum router.

* **Windows:** `ipconfig` (Look for IPv4 Address, e.g., `192.168.1.15`)
* **Mac/Linux:** `ifconfig` or `ip a`

**Write this IP on the board. Students need it for the `serverIP` variable.**

### 2. Instructor: Start Server

Run the python script:

```bash
python server.py
```

*Note: If a Windows Firewall popup appears, you **MUST** allow access for Private Networks.*

### 3. Students: Upload & Connect

1.  Ensure `secrets.h` is configured.
2.  Update the `serverIP` in the main `.ino` file.
3.  Upload code to ESP32.
4.  Open the Serial Monitor (115200 baud) to verify connection.

### 4. Verify

Open a browser on the instructor's computer and go to: `http://localhost:8080`
You should see devices appearing in real-time.

## Troubleshooting

| Issue | Possible Cause | Solution |
| :--- | :--- | :--- |
| **HTTP Error -1** | ESP32 cannot find server | Check `serverIP`. Ensure Host firewall allows port 8080. Ensure both devices are on the same router. |
| **HTTP Error 403** | Security Blocked | Check `CLASS_TOKEN` in `.env` (server) matches `secrets.h` (client). |
| **WiFi Connect Failed** | Wrong Credentials | Verify SSID/Password in `secrets.h`. |
| **No devices on Dashboard** | Browser not refreshing | The page auto-refreshes every 2s. Try manual refresh (F5). |
| **Import Error** | Missing Python Libs | Run `pip install python-dotenv flask` |
