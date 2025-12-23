# Simple HW alarm

## December 2025

### Description

The goal of the project is to create firmware for the FITkit 3 platform that
functions as a home security system. It monitors multiple sensors and triggers
an audiovisual alarm upon intrusion, with authentication provided via keypad or
RFID. Detailed specification

Implementation base: Suitable sensors and actuators – 4x4 membrane keypad,
RC-522 RFID module, Reed module, PIR sensor, Tilt sensor, LED traffic light, and
Piezo buzzer. Embedded platform: FITkit3. Language: C. IDE: Kinetis Design
Studio.

Solution requirements:

- Mandatory: Creation of a simple embedded alarm triggered by one of at least
  three different external events (using PIR, Reed, and Tilt sensors) and
  turnable off via keypad and RFID reader; appropriately signal the alarm state
  (armed/disarmed, standby, triggered) using at least two external components
  (LED traffic light and piezo buzzer).

- Optional: Support for different zones and modes.

### Examples of running

Arming the system (Mode D - All sensors) User enters *D1234 on keypad.

    Result: Long beep, Yellow LED blinks (exit delay), then Yellow LED stays ON (Armed).

Triggering Alarm System is Armed -> User trips the PIR sensor.

    Result: Red LED turns ON, Piezo buzzer plays siren tone.

Disarming System is in Alarm -> User swipes RFID card or enters *1234.

    Result: Long beep, Siren stops, Green LED turns ON (Idle).

Failed Auth User enters *9999.

    Result: Red LED blinks twice (Error), system remains in previous state.
