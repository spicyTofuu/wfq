# Weighted Fair Queueing (WFQ) Simulation

## Overview

This project implements a simulation of the Weighted Fair Queueing (WFQ) algorithm. WFQ is a packet scheduling algorithm that provides fair bandwidth distribution among different network flows. It is particularly useful in integrated services networks, where traffic is heterogeneous and requires different quality of service (QoS) guarantees.

The simulation reads packet data from a JSON file, processes the packets according to WFQ principles, and calculates both virtual and real finish times for each packet. The output is then written to another JSON file, providing a detailed overview of the packet scheduling process.

## Features

- **Fair Queueing**: Implements WFQ to ensure fair bandwidth allocation.
- **Virtual and Real Finish Time Calculation**: Calculates and records the finish times for each packet.
- **JSON Input/Output**: Reads input data from a JSON file and writes output to another JSON file for easy analysis.

## How to Run the Simulation

### Prerequisites

- C++ compiler (g++ or similar)
- Make (for building the project using the Makefile)
- `nlohmann/json` library (included in the project or installed separately)

### Compilation

1. **Compile the Project**:
   Navigate to the project directory and run the following command:

make


This will compile the source code using the provided Makefile.

2. **Run the Simulation**:
After compilation, run the simulation with the following command:

make run FILENAME="input_filename.json"


Replace `input_filename.json` with the path to your input JSON file.

### Input File Format

The input file should be a JSON file with the structure as in ./input.json


### Output File

The simulation generates an output JSON file named `output.json`, which contains the virtual and real finish times for each packet:


### Example

Assuming you have an input file named `input.json`, run the simulation as follows:

make run FILENAME="input.json"


Check the `output.json` file for the results of the simulation.

## Conclusion

This WFQ simulation offers a practical demonstration of weighted fair queueing in network packet scheduling. It's an excellent tool for studying the behavior of WFQ in different network scenarios.
