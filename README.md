# TELESim

TELESim is a C++20 telecommunication simulator that models communication between Phone, Radio, and Master devices using HTTP and JSON.

## Features

- **Master:** Manages phone ownership and device information.
- **Radio:** Handles call establishment, connections, and packet forwarding.
- **Phone:** Initiates calls and exchanges messages.
- **HTTP communication:** Uses REST-style endpoints and JSON payloads.
- **Multithreading:** Uses threads, mutexes, and condition variables for packet processing.
- **Logging:** Records communication events and errors.

## Tech Stack

- C++20
- CMake
- [cpp-httplib](https://github.com/yhirose/cpp-httplib)
- [nlohmann/json](https://github.com/nlohmann/json)

## Project Structure

```text
TELESim/
├── devices/       # Master, Radio, Phone and device information
├── fwk/           # Device factories and application coordination
├── HTTP_view/     # HTTP endpoints, listeners and emitters
├── libs/          # HTTP and JSON libraries
├── logs/          # Application logs
├── utils/         # Logging and utility functions
├── APIDocs.json   # API documentation
├── main.cpp       # Main application entry point
└── DeviceWindow.cpp
```

## Getting Started

### Prerequisites

- C++20-compatible compiler
- CMake
- Ninja (if using the Ninja generator)

### Build

```bash
git clone https://github.com/StoicavUPT/TELESim.git
cd TELESim

cmake -S . -B cmake-build-debug -G Ninja
cmake --build cmake-build-debug
```

Run the `TELESim` executable to start the application.

## API

The application uses HTTP endpoints for device registration, call initialization, connection management, and packet exchange.

See [`APIDocs.json`](APIDocs.json) for API request schemas and JSON structures.

## Overview

TELESim simulates how a basic telecommunications network operates. Phones initiate calls, Radios coordinate connections and forward messages, and the Master keeps track of phone ownership. The devices communicate through HTTP requests, while multithreading enables packets to be processed concurrently.

The project explores object-oriented design, network communication, and synchronization in C++.
