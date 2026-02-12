import os
from flask import Flask, request, jsonify, render_template_string
from dotenv import load_dotenv # Import this

# Load secrets from the .env file
load_dotenv()

app = Flask(__name__)

# --- CONFIGURATION ---
# Now we pull from os.getenv instead of hardcoding
CLASS_TOKEN = os.getenv("CLASS_TOKEN")
REQUIRED_USER_AGENT = "ESP32-Class-Device"

# Store connected devices in memory {mac_address: {data}}
connected_devices = {}

# HTML Template for the Dashboard
HTML_TEMPLATE = """
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Class Hub</title>
    <meta http-equiv="refresh" content="2"> <style>
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
    """Security check for User-Agent and Token."""
    agent = req.headers.get('User-Agent', '')
    token = req.headers.get('X-Auth-Token', '')
    
    if agent != REQUIRED_USER_AGENT:
        return False
    if token != CLASS_TOKEN:
        return False
    return True

@app.route('/')
def index():
    """The Dashboard page."""
    # Sort devices by most recently seen
    sorted_devices = dict(sorted(connected_devices.items(), key=lambda item: item[1]['last_seen'], reverse=True))
    return render_template_string(HTML_TEMPLATE, devices=sorted_devices)

@app.route('/connect', methods=['POST'])
def connect():
    """Endpoint for ESP32s to check in."""
    if not verify_request(request):
        return jsonify({"status": "denied", "reason": "Security Check Failed"}), 403

    try:
        # Expecting JSON: {"id": "StudentName", "mac": "AA:BB:CC...", "msg": "Hello"}
        data = request.json
        if not data:
            return jsonify({"status": "error", "reason": "No JSON data"}), 400

        # Sanitize inputs to prevent HTML injection on the dashboard
        device_id = html.escape(data.get("id", "Unknown"))
        mac_addr = html.escape(data.get("mac", "Unknown"))
        message = html.escape(data.get("msg", "No Data"))

        # Update the registry
        connected_devices[mac_addr] = {
            "id": device_id,
            "message": message,
            "last_seen": datetime.datetime.now().strftime("%H:%M:%S")
        }

        print(f"[+] Device Connected: {device_id} ({mac_addr})")
        return jsonify({"status": "accepted", "server_time": datetime.datetime.now().strftime("%H:%M:%S")}), 200

    except Exception as e:
        print(f"[-] Error: {e}")
        return jsonify({"status": "error"}), 500

if __name__ == '__main__':
    # Host 0.0.0.0 makes it accessible to the entire local network
    app.run(host='0.0.0.0', port=8080, debug=True)
