# v3.0 PCB Readme

## Overview
This document provides essential information regarding the v3.0 PCB (Printed Circuit Board) design, including features, specifications, and usage instructions.
![PCB rendering](https://github.com/MicrochipTech/veryVerilog/raw/main/doc/images/pcb.png)
## Features
- **Compact Design**: The v3.0 PCB is designed to be space-efficient while maintaining functionality.
- **Power Management**: Improved power distribution for optimal performance and reduced heat generation.

## Specifications
- **Dimensions**: 57.4mm x 24.89mm
- **Layer Count**: 2 layers
- **Material**: FR-4
- **Operating Voltage**: 5V (from USB A)
- **Current Rating**: 100mA without external application

## Schematic
Refer to the attached schematic diagram for detailed connections and component placements.

## Assembly Instructions
1. **Component Placement**: Follow the provided schematic for correct component placement. Use IBOM from miniFPGA/bom/miniFPGA?ibom.html
2. **Soldering**: Use appropriate soldering techniques to ensure reliable connections.
3. **Testing**: After assembly, U2 LEDs will shor a running light effect.

## Usage
- Connect the PCB USB-A port. Use the [web programmer application](https://microchiptech.github.io/veryVerilog)
- The lectures are on [Microchip try platform](https://try.microchip.com/training/logic-circuit-design-101)     

## Troubleshooting
- **No Power**: Check power supply connections and ensure the correct voltage is applied.
- **Component Failure**: Inspect for soldering issues or damaged components.

## License
This project is licensed under the MIT License - see the LICENSE file for details.
