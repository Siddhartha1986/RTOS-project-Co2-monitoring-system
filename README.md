
# CO2 Monitoring System for Greenhouses - Hackathon Project In Metropolia in collbration with Vaisala Oy

## Project Overview
This repository contains the source code for a CO2 Monitoring System developed during a Hackathon at Metropolia University of Applied Sciences. The project was created in collaboration with Vaisala Oy and aimed at controlling the CO2 level in greenhouses using a microcontroller-based system. This system integrates hardware control, real-time data monitoring, and cloud connectivity for effective greenhouse management.

## Key Features
- **CO2 Level Control**: Utilizes a rotary encoder and a valve to regulate CO2 flow into the greenhouse.
- **Real-Time Operating System (RTOS)**: Employs the principles of multitasking in an RTOS environment to manage multiple operations simultaneously.
- **Sensor Integration**: Incorporates CO2, temperature, and humidity sensors for comprehensive environmental monitoring.
- **Cloud Dashboard**: Leverages ThingSpeak, a free IoT Cloud platform, to visualize sensor readings and valve states in real-time.
- **Hardware Platform**: Built on the LPC 1549 microcontroller, providing robust and reliable hardware control.

## Repository Structure
- The main C source file containing the core logic and RTOS task definitions for the CO2 monitoring system.
- [HACKATHON.cpp](https://github.com/Siddhartha1986/RTOS-project-Co2-monitoring-system/blob/master/HACKATHON/src/HACKATHON.cpp): Contains the detailed implementation of each component, including MQTT communication, LCD display management, and sensor readings.

## Getting Started
### Prerequisites
- LPC1549 microcontroller board.
- Appropriate sensors and actuators (CO2 sensor, temperature sensor, humidity sensor, rotary encoder, valve).
- Development environment set up for LPC1549 (e.g., LPCXpresso IDE).

### Installation and Setup
1. Clone the repository to your local machine.
2. Ensure all hardware components are correctly connected to the LPC1549 board.
3. Open the project in your LPCXpresso IDE or equivalent development environment.
4. Compile and upload the code to the LPC1549 microcontroller.

## Usage
- The system will start monitoring the environmental parameters upon powering up.
- CO2 levels can be adjusted manually using the rotary encoder.
- Real-time data is sent to the ThingSpeak dashboard for monitoring.
- The system status and sensor readings are displayed on the connected LCD.

## Contributions
This project is the result of collaborative efforts during the Hackathon. We welcome further contributions and enhancements. Feel free to fork this repository and submit your pull requests or open issues to discuss potential improvements.

## Acknowledgements
Special thanks to Vaisala Oy for their support and collaboration in this project, and to Metropolia University of Applied Sciences for hosting the Hackathon.

## License
This project is licensed under the terms of the MIT license.

For detailed explanation of the Project refer [Documentaion](https://github.com/Siddhartha1986/RTOS-project-Co2-monitoring-system/blob/master/Documentation.pdf)
