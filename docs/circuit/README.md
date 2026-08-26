# Circuit Design & Peripheral Allocation

This document describes the hardware architecture of the Solar Panel Sun Tracker (SPST), including the ATmega32 peripheral allocation, sensor interfaces, actuator control, communication interfaces, and shared buses.

## System Overview

The SPST circuit is built around an ATmega32 microcontroller running at 16 MHz. The microcontroller interfaces with two LDR-based light sensors, a servo motor, an I²C LCD, a DS1307 RTC, status LEDs, and a UART serial terminal.

The circuit is designed in SimulIDE and serves as the hardware foundation for the project's firmware.

## Microcontroller Configuration

### ATmega32

| Configuration | Value |
| :--- | :--- |
| Microcontroller | ATmega32 |
| Clock Frequency | 16 MHz |
| ADC | 10-bit |
| Communication | TWI (I²C), USART |
| Timer/PWM | Hardware timer |
| Non-volatile Storage | Internal EEPROM |

## Peripheral Allocation

The ATmega32 peripherals are allocated according to the requirements of each hardware component.

| Peripheral | Purpose | Connected Component |
| :--- | :--- | :--- |
| ADC | Light measurement | East / West LDR |
| Timer/PWM | Servo control | Servo motor |
| TWI / I²C | Display and timekeeping | LCD + DS1307 |
| USART | System logging | Serial terminal |
| GPIO | Status indication | Status LEDs |
| EEPROM | Configuration storage | Internal |

> Exact physical pin assignments and wire-to-wire connections are documented in [connections.md](./connections.md).

## Hardware Interfaces

### Light Sensors

Two LDR voltage-divider circuits are used to measure the relative light intensity from the east and west sides of the panel.

The LDR arrangement is selected so that an increase in illumination produces an increase in the corresponding ADC reading.

### Servo Motor

The servo is controlled using a hardware PWM output from the ATmega32.

The firmware will control the panel position within the defined travel limits.

### I²C Bus

The LCD and DS1307 RTC share the ATmega32's hardware I²C/TWI bus.

Both devices use the same SDA and SCL lines and are distinguished by their I²C addresses.

External pull-up resistors are provided for the SDA and SCL lines.

### UART

The ATmega32 USART is connected to the SimulIDE serial terminal for system and debugging messages.

The project uses a baud rate of 9600.

### Status LEDs

Three LEDs provide visual indication of the system state during operation.

## Circuit Design Constraints

The circuit follows the hardware requirements defined by the SPST project specification:

- ATmega32 operates at 16 MHz.
- LDR voltage dividers must produce increasing ADC readings with increasing light.
- The LCD and DS1307 share the hardware I²C bus.
- SDA and SCL require pull-up resistors.
- The servo is driven using a hardware PWM output.
- UART communication operates at 9600 baud.
- ADC reference and supply connections must be correctly connected.
- The servo must remain within the defined mechanical travel limits.

## Detailed Connections

For the complete physical pin mapping and component connections, see:

**[→ Detailed Circuit Connections](./connections.md)**