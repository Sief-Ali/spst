# SPST Platform Layers

The application documentation is the primary reference. This page summarizes
the supporting HAL and MCAL code beneath it.

## HAL overview

HAL modules provide device-oriented operations for application code.

| Module | Application-facing responsibility |
|---|---|
| `analog` | Read the east and west LDR values. |
| `servo` and `panel` | Convert requested panel movement into servo pulse width and enforce travel limits. |
| `lcd` | Write state and sensor information to the I²C LCD. |
| `rtc` | Read the DS1307 time through I²C. |
| `serial` / `logger` | Send UART boot, error, and state-change messages. |
| `led` | Show the current high-level state. |

HAL does not contain tracker decisions or FreeRTOS task policy.

## MCAL overview

MCAL modules configure ATmega32 peripherals through registers.

| Module | Peripheral role |
|---|---|
| `adc` | Two 10-bit LDR input channels. |
| `gpio` | LEDs, servo PWM pin direction, and other digital pins. |
| `timer` | Timer register helpers; Timer0 is reserved for the FreeRTOS tick. |
| `twi` | Shared I²C bus for LCD and DS1307. |
| `uart` | 9600-baud serial terminal. |
| `eeprom` | Persistent tracker parameters. |
| `ext_int` | External-interrupt support when needed. |

## Boundary rules

```text
Application: what the tracker should do
HAL:         what each connected device should do
MCAL:        how the ATmega32 peripheral registers do it
```

For example, the Control task chooses `Panel_Park(20)`. The panel and servo
modules translate that command to a Timer1 compare value. The application does
not write `OCR1A` itself.

---

## Next and related documentation

- **Previous:** [Application architecture](application.md) — tracker behavior,
  tasks, FSM, and synchronization.
- [Software documentation index](README.md) — source layout and reading order.
- [Circuit documentation](../circuit/README.md) — hardware allocation and
  wiring constraints.
