# SPST Circuit Connections

This document defines the physical connections between the ATmega32 and the peripheral components used in the Solar Panel Sun Tracker (SPST) circuit.

The ATmega32 operates at **16 MHz**.

---

## 1. UART / Serial Terminal

The ATmega32 hardware USART is connected to the Serial Terminal for system logging and debugging.

The UART communication speed is **9600 baud**.

| ATmega32 Pin | Peripheral | Serial Terminal | Connection |
| :--- | :--- | :--- | :--- |
| **PD0** | `RXD` | `TX` | Terminal TX → MCU RX |
| **PD1** | `TXD` | `RX` | MCU TX → Terminal RX |
| **GND** | Ground | `GND` | Common ground |


```text
ATmega32                         Serial Terminal

PD1 / TXD  ───────────────────►  RX
PD0 / RXD  ◄───────────────────  TX
GND        ─────────────────────  GND
````

---

## 2. Servo Motor

The servo receives its control signal from the ATmega32's hardware Timer1 PWM output.

| ATmega32 Pin | Peripheral                | Servo Pin | Connection    |
| :----------- | :------------------------ | :-------- | :------------ |
| **PD5**      | `OC1A` / Timer1 Channel A | Signal    | PWM control   |
| **VCC**      | Supply                    | VCC       | Servo power   |
| **GND**      | Ground                    | GND       | Common ground |

```text
ATmega32

PD5 / OC1A ───────────────────► Servo Signal
VCC        ───────────────────► Servo VCC
GND        ───────────────────► Servo GND
```

The servo is controlled using hardware PWM rather than software-generated pulses.

---

## 3. I²C / TWI Bus

The ATmega32 hardware TWI interface is shared by the **16×2 I²C LCD** and the **DS1307 RTC**.

### ATmega32 → I²C Bus

| ATmega32 Pin | Peripheral | Bus Line |
| :----------- | :--------- | :------- |
| **PC1**      | `SDA`      | SDA      |
| **PC0**      | `SCL`      | SCL      |

Both I²C devices connect to the same SDA and SCL bus lines.

### LCD and DS1307

| Device                | SDA     | SCL     |
| :-------------------- | :------ | :------ |
| **I²C LCD / PCF8574** | SDA bus | SCL bus |
| **DS1307 RTC**        | SDA bus | SCL bus |

```text
                         ┌──► LCD SDA
ATmega32 PC1 / SDA ──────┼──► DS1307 SDA
                         │
                         └── SDA BUS

                         ┌──► LCD SCL
ATmega32 PC0 / SCL ──────┼──► DS1307 SCL
                         │
                         └── SCL BUS
```

### I²C Pull-up Resistors

Both bus lines are pulled up to VCC using **4.7 kΩ resistors**.

```text
             VCC
              │
           ┌──┴──┐
         4.7kΩ  4.7kΩ
           │      │
           │      │
          SDA    SCL
           │      │
           │      │
        PC1/SDA  PC0/SCL
```

The pull-up resistors are shared by both I²C devices.

---

## 4. East LDR

The East LDR is connected to **ADC0 (PA0)**.

The LDR forms a voltage divider with a **10 kΩ resistor**.

The divider is arranged so that increasing light intensity produces an increasing ADC reading.

```text
VCC
 │
[LDR - East]
 │
 ├──────────────► PA0 / ADC0
 │
[10 kΩ]
 │
GND
```

| Connection         | Destination             |
| :----------------- | :---------------------- |
| LDR upper terminal | VCC                     |
| LDR lower terminal | ADC0 signal node        |
| ADC0 signal node   | ATmega32 **PA0 / ADC0** |
| 10 kΩ resistor     | ADC0 signal node → GND  |

---

## 5. West LDR

The West LDR is connected to **ADC1 (PA1)**.

It uses the same voltage-divider arrangement as the East LDR.

```text
VCC
 │
[LDR - West]
 │
 ├──────────────► PA1 / ADC1
 │
[10 kΩ]
 │
GND
```

| Connection         | Destination             |
| :----------------- | :---------------------- |
| LDR upper terminal | VCC                     |
| LDR lower terminal | ADC1 signal node        |
| ADC1 signal node   | ATmega32 **PA1 / ADC1** |
| 10 kΩ resistor     | ADC1 signal node → GND  |

---

## 6. Status LEDs

Three LEDs are used for system-state indication.

The LEDs are connected to unused GPIO pins on **Port B**.

| ATmega32 Pin | GPIO | LED          |
| :----------- | :--- | :----------- |
| **PB0**      | GPIO | Status LED 1 |
| **PB1**      | GPIO | Status LED 2 |
| **PB2**      | GPIO | Status LED 3 |

Each LED is connected in series with a current-limiting resistor.

```text
PB0 ──[Current-Limiting Resistor]──► LED 1 ──► GND

PB1 ──[Current-Limiting Resistor]──► LED 2 ──► GND

PB2 ──[Current-Limiting Resistor]──► LED 3 ──► GND
```

The exact LED-to-system-state mapping is defined by the firmware.

---

## 7. Power and Ground

All components share a common ground.

The ATmega32, LDR circuits, servo, LCD, DS1307, status LEDs, and UART interface must be connected to the appropriate supply and common ground.

The ATmega32 ADC supply and reference connections must also be connected correctly for valid ADC operation.

---

## 8. Complete Peripheral Mapping

| ATmega32 Port | Pin Function | Connected Component | Purpose                |
| :------------ | :----------- | :------------------ | :--------------------- |
| **PA0**       | `ADC0`       | East LDR            | East light measurement |
| **PA1**       | `ADC1`       | West LDR            | West light measurement |
| **PB0**       | GPIO         | Status LED 1        | State indication       |
| **PB1**       | GPIO         | Status LED 2        | State indication       |
| **PB2**       | GPIO         | Status LED 3        | State indication       |
| **PC0**       | `SCL`        | LCD + DS1307        | I²C clock              |
| **PC1**       | `SDA`        | LCD + DS1307        | I²C data               |
| **PD0**       | `RXD`        | Serial Terminal TX  | UART receive           |
| **PD1**       | `TXD`        | Serial Terminal RX  | UART transmit          |
| **PD5**       | `OC1A`       | Servo Signal        | Timer1 PWM             |

---

## 9. Circuit Overview

```text
                           ATmega32
                         ┌───────────┐
                         │           │
 East LDR ──────────────►│ PA0 ADC0  │
                         │           │
 West LDR ──────────────►│ PA1 ADC1  │
                         │           │
 LED 1 ◄─────────────────│ PB0       │
 LED 2 ◄─────────────────│ PB1       │
 LED 3 ◄─────────────────│ PB2       │
                         │           │
 LCD ◄──────────────────►│ PC1 SDA   │
 DS1307 ◄───────────────►│ PC0 SCL   │
                         │           │
 Servo ◄─────────────────│ PD5 OC1A  │
                         │           │
 Terminal TX ───────────►│ PD0 RXD   │
 Terminal RX ◄───────────│ PD1 TXD   │
                         │           │
                         └───────────┘
```

---

## Related Documentation

* [Circuit Design & Peripheral Allocation](./README.md)
