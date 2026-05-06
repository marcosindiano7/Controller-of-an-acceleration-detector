# Controller of an Acceleration Detector

Embedded acceleration detection system developed on an **STM32F429Zi NUCLEO** board using **CMSIS-RTOS2 (RTX)**.

The project implements a real-time controller capable of reading acceleration values from an **MPU6050 accelerometer/gyroscope sensor**, monitoring motion along the X, Y and Z axes, displaying system information on an LCD, indicating acceleration events through the NUCLEO user LEDs, and allowing configuration/debugging through a UART command protocol.

## Overview


The main objective was to design and implement a complete embedded application based on a real-time operating system, integrating sensor acquisition, user interaction, visual feedback, serial communication and modular firmware architecture.

The system continuously monitors acceleration values measured by the MPU6050 sensor. These values are compared with configurable reference thresholds for each axis. When the acceleration measured on a given axis exceeds its reference value, the corresponding NUCLEO LED is activated.

The system also reads and displays the temperature provided by the MPU6050 sensor.

## Main Features

- Real-time embedded application using **CMSIS-RTOS2 / RTX**
- Modular firmware architecture
- Acceleration monitoring in three axes: **X, Y and Z**
- Temperature acquisition from the **MPU6050**
- Sensor configuration for **±2g acceleration range**
- Periodic sensor readings every **1 second**
- System clock with **1-second resolution**
- Three operating modes:
  - Standby mode
  - Active mode
  - Programming/debugging mode
- LCD-based user interface
- Joystick control with:
  - Interrupt-based input handling
  - Debouncing
  - Short and long press detection
- NUCLEO user LEDs used as axis threshold indicators:
  - X axis → LD1
  - Y axis → LD2
  - Z axis → LD3
- Circular buffer for storing recent measurements
- UART/RS232 command protocol for configuration and debugging
- Interrupt/event-based UART communication using CMSIS USART driver

## Hardware Used

- **NUCLEO STM32F429Zi**
- **mbed Application Board**
- **MPU6050 accelerometer/gyroscope sensor**
- LCD display
- Joystick
- NUCLEO user LEDs:
  - LD1
  - LD2
  - LD3
- UART connection to PC through USB/RS232
- PC terminal software for command testing

## Software and Tools

- **C**
- **Keil µVision**
- **STM32 HAL**
- **CMSIS-RTOS2 / RTX**
- **CMSIS USART Driver**
- **I2C**
- **SPI**
- **UART / RS232**
- **GPIO interrupts**
- **Message Queues**
- **Thread Flags**
- **Tera Term** for UART command testing

## System Modes

### Standby Mode

After reset, the system starts in standby mode.

The LCD displays the system title and the current time, which starts from `00:00:00`.

The system remains in this mode until a long press on the joystick center button changes the state to active mode.

### Active Mode

In active mode, the system reads the acceleration and temperature values from the MPU6050 sensor every second.

The measured acceleration values are:

```text
Ax, Ay, Az
```

These values are compared with the reference acceleration thresholds:

```text
Ax_r, Ay_r, Az_r
```

By default, the reference values are:

```text
Ax_r = 1.0
Ay_r = 1.0
Az_r = 1.0
```

If the measured acceleration on an axis is higher than its reference value, the corresponding LED is switched on.

| Axis | Reference Value | LED Indicator |
|---|---:|---|
| X axis | Ax_r | LD1 |
| Y axis | Ay_r | LD2 |
| Z axis | Az_r | LD3 |

If the measured acceleration is below the reference value, the corresponding LED is switched off.

The LCD displays the system mode, the temperature measured by the sensor and the acceleration values for the three axes.

Example display:

```text
ACTIVO-- T:21.5º
X:n.n Y:n.n Z:n.n
```

### Programming and Debugging Mode

This mode allows the user to configure the system parameters.

The following values can be modified:

- System time
- X-axis acceleration reference value
- Y-axis acceleration reference value
- Z-axis acceleration reference value

The configuration can be performed using the joystick with short presses and directional gestures.

The system can also be controlled from a PC through the UART/RS232 command protocol.

From this mode, the system can return to standby mode by performing a long press on the joystick center button.

## Acceleration Detection Logic

The controller compares the acceleration measured in each axis with its corresponding reference value.

```text
If Ax > Ax_r → LD1 ON
If Ay > Ay_r → LD2 ON
If Az > Az_r → LD3 ON
```

If the measured value is lower than or equal to the reference value, the corresponding LED remains off.

This allows the system to detect acceleration events independently in each axis.

## UART Communication Protocol

The system can be controlled from a PC using a UART/RS232 communication channel.

UART configuration:

```text
Baud rate: 115200
Data bits: 8
Stop bits: 1
Parity: None
```

All frames follow this format:

```text
SOH CMD LEN Payload EOT
```

Where:

| Field | Description |
|---|---|
| SOH | Start of frame, `0x01` |
| CMD | Command byte |
| LEN | Total frame length, including SOH and EOT |
| Payload | ASCII-encoded command data |
| EOT | End of frame, `0xFE` |

Malformed frames are ignored by the system.

## Supported UART Commands

| Command | CMD | Description |
|---|---:|---|
| Set system time | `0x20` | Sets the internal clock |
| Set X-axis reference acceleration | `0x25` | Updates `Ax_r` |
| Set Y-axis reference acceleration | `0x26` | Updates `Ay_r` |
| Set Z-axis reference acceleration | `0x27` | Updates `Az_r` |
| Request all measurements | `0x55` | Sends all stored measurements from the circular buffer |
| Clear measurements | `0x60` | Clears the circular measurement buffer |

## UART Responses

When a valid command is received, the system responds with the corresponding response frame.

| Response | CMD |
|---|---:|
| Time set response | `0xDF` |
| X-axis reference set response | `0xDA` |
| Y-axis reference set response | `0xD9` |
| Z-axis reference set response | `0xD8` |
| Measurement response | `0xAF` |
| Clear measurements response | `0x9F` |

## Measurement Buffer

In active mode, the system stores the last 10 measurements in a circular buffer.

Each stored entry follows this format:

```text
HH:MM:SS--Tm:TT.Tº-Ax:n.n-Ay:n.n-Az:n.n
```

Where:

- `HH:MM:SS` = timestamp
- `Tm` = measured temperature from the MPU6050
- `Ax` = measured acceleration on the X axis
- `Ay` = measured acceleration on the Y axis
- `Az` = measured acceleration on the Z axis

The stored measurements can be requested from the PC using the UART command `0x55`.

## Software Architecture

The firmware was designed following a modular approach.

Each module is responsible for one specific peripheral, sensor or functionality. The main module coordinates all the others and controls the global behaviour of the system.

## Main Software Modules

### Clock Module

Responsible for maintaining the system time with 1-second resolution.

Typical files:

```text
clock.c
clock.h
```

### MPU6050 Module

Responsible for reading acceleration and temperature values from the MPU6050 sensor through I2C.

Typical files:

```text
mpu6050.c
mpu6050.h
```

### Joystick Module

Responsible for detecting joystick inputs using interrupts.

It identifies:

- Directional gestures
- Center button presses
- Short presses
- Long presses
- Debounced input events

Typical files:

```text
joystick.c
joystick.h
```

### NUCLEO LEDs Module

Responsible for switching LD1, LD2 and LD3 on or off depending on the acceleration threshold comparison.

Typical files:

```text
leds_N.c
leds_N.h
```

### LCD Module

Responsible for displaying system information through the LCD connected by SPI.

Typical files:

```text
lcd.c
lcd.h
```

### COM-PC Module

Responsible for UART communication with the PC.

It manages:

- Frame reception
- Frame validation
- Command decoding
- Response transmission
- Communication through interrupt/event-based USART callbacks

Typical files:

```text
com.c
com.h
```

### Main Controller Module

Responsible for coordinating the complete system.

It manages:

- Operating mode transitions
- Sensor data processing
- LED activation logic
- LCD updates
- UART command execution
- Circular buffer management
- Communication between modules using RTOS mechanisms

Typical files:

```text
principal.c
principal.h
```

## RTOS Synchronization

The project uses several CMSIS-RTOS2 mechanisms to coordinate the application modules.

Main synchronization mechanisms:

- **Threads** for concurrent module execution
- **Message Queues** for sending data between modules
- **Thread Flags** for LED state control
- **Global variables** for system time management
- **Interrupt callbacks** for joystick and UART events

This design improves modularity and separates peripheral management from the main application logic.

## Skills Demonstrated

This project demonstrates practical experience in:

- Embedded C programming
- STM32 firmware development
- Real-time operating systems
- CMSIS-RTOS2 application design
- Modular embedded software architecture
- I2C sensor communication
- MPU6050 sensor integration
- SPI LCD control
- UART protocol design
- Interrupt-driven programming
- Message queue-based synchronization
- Thread flag-based event control
- Circular buffer implementation
- Embedded systems debugging
- Hardware/software integration

## Author

**Marcos Indiano**  
Electronic Engineering Student  
Universidad Politécnica de Madrid
