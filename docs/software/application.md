# SPST Application Architecture

## Purpose

The application controls a solar panel using two LDR readings. It tracks the
brighter side during the day, holds position during a cloud, parks east at
night, and stops movement for invalid daytime readings.

## Timer allocation

| Resource | Allocation | Configuration | Reason |
|---|---|---|---|
| FreeRTOS system tick | Timer0 Compare Match | 16 MHz, prescaler `/64`, `OCR0 = 249` | Exact 1 ms tick. |
| Servo PWM | Timer1 Compare A / OC1A | 16 MHz, prescaler `/8`, `ICR1 = 39999` | Exact 20 ms / 50 Hz frame and fine pulse resolution. |
| Timer2 | Unused | — | Reserved for a later feature. |

Timer0 gives `16,000,000 / 64 / (249 + 1) = 1,000 Hz`. Timer1 gives
`16,000,000 / 8 / (39,999 + 1) = 50 Hz`. This separation prevents FreeRTOS
from changing the servo timer registers.

## FreeRTOS tasks

| Task | Period / trigger | Single responsibility |
|---|---|---|
| Sensor | Every 100 ms | Read both LDR ADC channels and send the pair to the queue. |
| Control | Queue receive | Run the FSM and issue all servo movement, hold, or park commands. |
| Display | Every 500 ms; RTC every 1 s | Read RTC time, update LCD/LEDs, and log state changes over UART. |

```text
East LDR ─┐
          ├─ Sensor Task ─ queue ─► Control Task ─► FSM / Servo
West LDR ─┘                               │
                                           ▼
                                protected status snapshot
                                           ▲
              Display Task ─ LCD / RTC / UART / LEDs
```

## Synchronization and data ownership

- The LDR reading pair is sent with `xQueueSend()` and received with
  `xQueueReceive()`. It is not transferred through a shared global variable.
- The binary I²C semaphore is created before the scheduler and given once so
  it begins available.
- The Display task takes the semaphore before every complete RTC or LCD I²C
  transaction and gives it after the transaction finishes.
- RTC time and the display/control status snapshot are copied inside FreeRTOS
  critical sections. This prevents a task from reading half-updated data.
- The Control task is the only task allowed to command the servo.

## State machine

The controller has five explicit states:

| State | Action | Main transition condition |
|---|---|---|
| `INIT` | Start application objects and read the initial time. | Day → `TRACKING`; night → `NIGHT_PARKING`. |
| `TRACKING` | Move 5° toward the brighter LDR outside the dead-band. | Night, fault, or confirmed cloud condition. |
| `CLOUD_HOLD` | Hold the current angle. | Light reaches 550 counts → `TRACKING`; night has priority. |
| `NIGHT_PARKING` | Park at 20° east. | Day window → `TRACKING`. |
| `FAULT` | Hold the servo at its current safe angle. | Valid daytime readings → `TRACKING`; night has priority. |

Priority is: `NIGHT_PARKING`, `FAULT`, `CLOUD_HOLD`, then `TRACKING`.

## Parameters and reporting

The fixed project values are read from internal EEPROM at startup: dead-band,
travel limits, east park angle, cloud thresholds and confirmation time, and
day/night window. The LCD shows state, panel angle, and both LDR values. UART
logs state changes at 9600 baud rather than continuously printing repeated
messages.

---

## Next and related documentation

- **Next:** [Platform layers](platform.md) — the HAL and MCAL code supporting
  this application.
- [Software documentation index](README.md) — software reading order and
  source layout.
- [Circuit connections](../circuit/connections.md) — physical pin mapping for
  the application peripherals.
