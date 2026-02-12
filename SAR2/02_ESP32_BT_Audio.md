# Project Name: ESP32 Bluetooth Audio
**Course:** ENGR 4321 Microcontrollers Embedded Systems  
**Student:** Daniel Critchlow Jr.  
**Date:** 9 Feb 2026

---

## Project Overview
This project involves building a Bluetooth-enabled audio system that leverages an ESP32 to wirelessly receive digital audio signals from external devices via the A2DP profile. The received data is then processed through the ESP32's internal DAC and amplified to drive a physical speaker, effectively transforming the microcontroller into a functional wireless audio sink.

---

## Hardware Components (Simulated)
* **Microcontroller:** ESP32 DevKit V1
* **Input:** ESP32 On-board Bluetooth (A2DP Sink)
* **Output:** HXJ8002 Audio Module and 2030 Speaker

---

## Simulation Link
* <a href="https://wokwi.com/projects/455782918003372033" target="_blank">Wokwi Lab link</a>
* <a href="https://docs.sunfounder.com/projects/esp32-starter-kit/en/latest/arduino/basic_projects/ar_bluetooth_audio_player.html" target="_blank">Lab content link</a>
---

## How It Works

### 1. Wireless Data Acquisition (Bluetooth A2DP)
The ESP32 operates as a Bluetooth **A2DP Sink**, establishing a wireless link with a source device to receive stereo audio data. This process is managed by the `BluetoothA2DPSink` library, which handles the complex Bluetooth stack and provides the digital audio stream to the system.

### 2. Digital Transport and Synthesis (I2S)
The received digital data is routed through the **I2S (Inter-IC Sound)** interface, configured in **Master Mode** and **DAC Built-In Mode**.

* **Sample Rate & Bit Depth:** The data is typically synchronized at **44.1kHz** with **16-bit** resolution to maintain standard audio fidelity.
* **Internal Routing:** Instead of sending data to external pins for an external DAC chip, the I2S hardware is internally mapped to the ESP32’s built-in digital-to-analog converters.



### 3. Digital-to-Analog Conversion (DAC)
The ESP32's **8-bit internal DAC** converts the digital PCM (Pulse Code Modulation) data into a varying analog voltage. This signal is output specifically on **GPIO 25** (Channel 1).

### 4. Signal Conditioning and Power Amplification
The raw analog signal undergoes two final stages before reaching the speaker:

* **Passive Filtering:** A **10K resistor** is placed in series to create a simple low-pass filter with the circuit's parasitic capacitance, effectively suppressing high-frequency noise and smoothing the output.
* **Active Amplification:** Because the internal DAC output is restricted to **1.1V**, it enters the **Audio Amplifier Module**. The amplifier increases the signal's current and voltage to a level capable of vibrating the speaker's diaphragm, producing audible sound.

## Discussion: Challenges and Behavior
A significant technical challenge encountered during simulation was the implementation of the custom `audio-amp.chip.c` logic. The Wokwi API version required strict alignment between the JSON pin definitions and the C initialization. Specifically, a build error was resolved by updating the `pin_watch` configuration to use the `.pin_change` field instead of `.callback` or `.on_change` to match the specific header definitions of the simulator environment.

Another interesting anomaly was the behavior of the pot we added. When we go from max to min turn, it will cut audio and pause the music player on the phone. Not sure how that happens but it was interesting behavior.

---

## Conclusion
This project successfully demonstrates the successful use of 32-bit hardware-software interaction to create a complex audio system. It highlights the versatility of the ESP32's internal peripherals, such as I2S and DAC, to handle real-time data streams without external dedicated silicon.



## Future Enhancements
* **External I2S DAC Integration:** Upgrading from the 8-bit internal DAC to a dedicated 24-bit external DAC (e.g., PCM5102A) would significantly improve signal-to-noise ratio and audio clarity.
* **Stereo Output Implementation:** While the current build is mono, the circuit can be expanded by adding a second amplifier and speaker to utilize both DAC channels (GPIO 25 and 26) for a full stereo experience.
* **Graphic Equalizer Display:** Integrating an I2C OLED display could allow for real-time visualization of the audio frequencies using Fast Fourier Transform (FFT) processing.
* **Volume Control Interface ():** Adding a physical rotary encoder or utilizing the existing potentiometer to adjust gain levels within the software would provide a more tactile user experience.
* **Battery Power Management:** Incorporating a LiPo battery and a charging circuit would make the system a truly portable wireless speaker solution.
