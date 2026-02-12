# ENGR 4321 Microcontrollers and Embedded Systems Spring 2026
## Lecture 2
- Jan 14 2026

## Course Overview

### Course Objectives
- Introduction to Embedded Systems and Physical Computing
    - Wokwi Simulation Demonstration

### Course Materials (Reading)
- [Chapter 1 : “Making Embedded Systems”, Elecia White, O’Reilly, 2nd Edition, (2024)](https://www.oreilly.com/library/viewmaking-embedded-systems/9781098151539/)
- [Chapter 1 : “A Beginner’s Guide to Designing Embedded System Applications on ARM Cortex-M Microcontrollers”, A. Lutenberg et. al., ARM Education, (2021)](https://www.arm.com/resources/education/books/designing-embedded-systems)

### Course Materials (Software)
- [WokWi Simulators](https://wokwi.com/)
- [GitHub(Personal)](https://github.com/JuniorAndChill)

---

### What Is an Embedded System?
- A computing system designed for a specific purpose
- Interacts with the physical world
- Combines hardware, software, and timing constraints
- Operates under limited resources

### Embedded Systems vs Traditional Computing
- General-purpose computers run many tasks
- Embedded systems perform dedicated functions
- Timing and reliability are critical
- Failure can impact the physical world

### Embedded System
At its core, an embedded system:
- Senses the physical world (through sensors),
- Processes information (using a microcontroller or processor),
- Acts on the environment (through outputs or actuators).

Embedded systems operate under constraints such as limited power, memory, processing speed, and strict timing requirements. Many run continuously and must respond correctly every time.

### Embedded Systems Examples
Examples include:
- Thermostat regulating temperature
- Car’s airbag controller
- Pacemaker monitoring heart rhythm

---

### Embedded Systems and Physical Computing
Steps to Design and Implement an Embedded System Project

| **Step** | **Outcome** |
| :--- | :--- |
| 1. Select the project that will be implemented | A rationale that leads to an appropriate project to implement |
| 2. Elicit project requirements and use cases | A concise and structured description of what will be implemented |
| 3. Design the hardware | Diagram of hardware modules, connections, and bill of materials |
| 4. Design the software | Diagram of the software design and description of the modules |
| 5. Implement the user interface | Software implementation |
| 6. Implement the reading of the sensors | Software implementation |
| 7. Implement the driving of the actuators | Software implementation |
| 8. Implement the system behavior | Software implementation |
| 9. Check the system behavior | Assessment of accomplishment of requirements and use cases |
| 10. Develop the final documentation | A reference to the most relevant documentation of the project |

---

### In-Class Activity - Wokwi Quick Start
1. Go to https://wokwi.com
2. Select 'Arduino Uno' (no account required to start)
3. Enter the following sketch
4. Click 'Start Simulation’
5. Observe LED behavior
6. Change delay values and observe timing effects

---
> This is the standard "Blink" sketch used to test that an Arduino board is functioning correctly. It targets the on-board LED (LED_BUILTIN), usually connected to digital pin 13, and toggles it on and off every second.
```cpp
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
```
---
```cpp
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600); // Start serial communication at 9600 baud}
}
// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
  // Send data to the computer
  Serial.println("Hello from Arduino!");
  delay(1000);

  // Receive data from the computer (if any)
  if (Serial.available()) {
    char incomingByte = Serial.read();
    Serial.print("Received: ");
    Serial.println(incomingByte);
  }
}
```
>[ArduinoUnoTestScript](https://wokwi.com/projects/453163112319467521) : https://wokwi.com/projects/453163112319467521

# Summary
- Embedded systems are **purpose-built, time-aware, and physically connected**, while traditional computers are **flexible, user-driven, and resource-rich.**
- Failures can have **real-world consequences**
- Embedded systems are designed to **sense, decide, and act**
