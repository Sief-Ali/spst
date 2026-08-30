# SPST Software Documentation

This documentation starts with the **application**: the Solar Panel Sun
Tracker behavior, FreeRTOS tasks, state machine, timing, and data flow.
HAL and MCAL are documented afterwards as the supporting layers that make the
application possible.

## Reading order

1. [Application architecture](application.md) — business logic, tasks, queue,
   semaphore, FSM, and Timer0/Timer1 allocation.
2. [Platform layers](platform.md) — HAL and MCAL responsibilities and module
   overview.
3. [Circuit documentation](../circuit/README.md) — component wiring and pin
   allocation.

## Software focus

```text
Application / business logic
├── FreeRTOS Sensor, Control and Display tasks
├── LDR queue and protected shared status
├── Five-state tracker FSM
├── Tracking, cloud, night, and fault decisions
├── Servo commands and reporting
└── EEPROM-loaded configuration
    │
    ▼
HAL
├── Analog, servo, LCD, RTC, LEDs, and serial interfaces
│
▼
MCAL
├── ADC, GPIO, Timer, TWI/I²C, UART, EEPROM, and interrupts
│
▼
ATmega32 registers and hardware
```

The dependency direction remains `APP → HAL → MCAL → hardware`. Application
code does not manipulate ATmega32 registers directly.

## Source layout

```text
src/
├── app/                         # Main project behavior
│   ├── app.c                    # FreeRTOS tasks and system coordination
│   ├── config.c                 # EEPROM parameter handling
│   ├── controller/              # Five-state FSM
│   ├── tracker/                 # LDR comparison and movement direction
│   ├── panel/                   # Travel limits and servo-facing panel API
│   └── logger/                  # UART event logging
├── board/                       # Selected pins and board-level objects
├── HAL/                         # Hardware-oriented interfaces
├── MCAL/                        # Register-level peripheral drivers
├── lib/
│   ├── FreeRTOS/                # Git submodule; FreeRTOS kernel and AVR port
│   └── config/FreeRTOSConfig.h  # FreeRTOS application configuration
└── utils/                       # Shared utilities
```

## Documentation map

| File | Focus |
|---|---|
| [application.md](application.md) | How the tracker behaves and how the RTOS application is organized. |
| [platform.md](platform.md) | Supporting HAL/MCAL modules and their responsibilities. |
| [README.md](../../README.md) | Clone, build, and SimulIDE run instructions. |

---

## Next and related documentation

- **Next:** [Application architecture](application.md) — start here for the
  tracker behavior, FreeRTOS tasks, and timing.
- [Platform layers](platform.md) — supporting HAL and MCAL module overview.
- [Circuit documentation](../circuit/README.md) — hardware allocation and
  wiring requirements.
- [Project README](../../README.md) — clone, build, and run instructions.
