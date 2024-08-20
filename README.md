# RPi-Pico-Drive

## Summary
- [Project Description](#project-description)
- [System Overview](#system-overview)
  - [Multi-Core Usage](#multi-core-usage)
  - [Data Scheme](#data-scheme)
  - [Pin Out](#pin-out)
  - [Waveform Output](#waveform-output)

## Project Description
`RPi-Pico-Drive` is a program for controlling a Raspberry Pi Pico. It is designed to manage waveform output through VVVF-Simulator.

## System Overview

### Multi-Core Usage
The program utilizes the multi-core capabilities of the Raspberry Pi Pico. Each core is assigned specific tasks:
- Core 0: Receives data through USB.
- Core 1: Sets pin status.

### Data Scheme
The program employs a data scheme to manage waveform phases. Each phase status is represented by two bits. Here's the corresponding value for phase status:
- 0: L1 and L2 are ON (0V)
- 1: H1 and L1 are ON (E/2V)
- 2: H2 and H1 are ON (EV)

To represent one phase status, it requires two bits, resulting in a one-byte scheme for transmission.

| Index | Description    |
|-------|----------------|
| 0     | W Phase Bit 0 |
| 1     | W Phase Bit 1 |
| 2     | V Phase Bit 0 |
| 3     | V Phase Bit 1 |
| 4     | U Phase Bit 0 |
| 5     | U Phase Bit 1 |

### Pin Out
The pinout configuration is detailed in `gpio_control.c`. This file specifies the mapping of pins to their respective functions.

### Waveform Output
The program includes a 40000-byte buffer to store received data from USB. It reads the buffer every 5 microseconds and sets the pins to the corresponding status based on the data scheme. When the reading index reaches the writing (receiving) index, the pin output will be all 0. If the data read from the index is `0xFF`, the output will be all 0, and the program will wait for new data to be received.

