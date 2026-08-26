<p align="center">
  <img src="https://img.shields.io/badge/Embedded%20System-Yes-yellow.svg" alt="Embedded System Badge">
  <img src="https://img.shields.io/badge/EmbeddedC-Enabled-blue.svg" alt="EmbeddedC Badge">
  <img src="https://img.shields.io/badge/Simulator-SimulIDE-ff69b4.svg" alt="SimulIDE Badge">
  <img src="https://img.shields.io/badge/Status-In%20Development-orange.svg" alt="Project Status Badge">
</p>

# Solar Panel Sun Tracker (SPST)

<p align="center">
  <img src="./docs/assets/main.png" alt="Solar Panel Sun Tracker SimulIDE Circuit" width="900">
</p>

Solar Panel Sun Tracker (SPST) is an embedded systems project developed for the **Digital Egypt Cubs Initiative (DECI) – Embedded Systems Level 5**.

The project simulates an automated solar panel sun tracker using an **ATmega32** microcontroller and **SimulIDE**. The system uses two LDR sensors to determine the direction of stronger light and controls a servo motor to orient the solar panel accordingly.

The project will progressively evolve from basic sensor-based tracking into a complete embedded application with time-based parking, cloud detection, fault handling, system monitoring, and FreeRTOS-based control.

## Features

- Servo-based panel positioning
- Configurable servo travel limits
- EEPROM-based configuration parameters
- Automatic night parking
- Cloud detection and hold behavior
- Sensor fault detection
- UART system logging (9600 baud)
- Status LED indication
- FreeRTOS-based architecture
- SimulIDE simulation support

---

## Documentation

Detailed documentation is organized under the `docs/` directory.

### Circuit Documentation

The circuit documentation contains the ATmega32 peripheral allocation, hardware architecture, design decisions, and detailed connection mapping.

**[→ Circuit Documentation](./docs/circuit/README.md)**

---

## Prerequisites

Install the following tools before building:

- AVR-GCC Toolchain
- CMake
- SimulIDE v1.1

---

## Getting Started

### Clone the Repository

```bash
git clone https://github.com/Sief-Ali/spst.git
cd spst
```

If you already have the repository and want the latest changes:

```bash
git pull origin master
```

---

## Building the Project

Configure the project:

```bash
cmake \
    -B build \
    -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/avr-gcc.cmake
```

Build the firmware:

```bash
cmake --build build
```

The generated firmware will be located at:

```text
build/src/spst.hex
```

---

## Running the Simulation

1. Launch **SimulIDE**.
2. Open the project:

```text
simulide/main.sim1
```

3. Load the generated firmware:

```text
build/src/spst.hex
```

4. Start the simulation.

---

## Project Structure

```text
.
├── src/
│   ├── app/
│   ├── board/
│   ├── hal/
│   ├── mcal/
│   └── utils/
├── cmake/
├── docs/
├── simulide/
└── CMakeLists.txt
```
