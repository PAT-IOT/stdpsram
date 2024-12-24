# ESP32 PSRAM Standard Containers Example

This project demonstrates how to use standard C++ containers such as `std::vector`, `std::list`, `std::map`, `std::string`, and `std::tuple` with PSRAM support on the ESP32 microcontroller. It leverages external PSRAM (Pseudo Static RAM) to extend the available memory for memory-intensive applications.

The code shows how to allocate and use PSRAM for various standard containers, making it ideal for projects that require more memory than the internal SRAM on the ESP32.

## Features

- Demonstrates the use of `stdpsram::vector`, `stdpsram::list`, `stdpsram::map`, `stdpsram::string`, and `stdpsram::tuple` with PSRAM.
- Provides examples of allocating these containers in PSRAM and performing basic operations like insertion and iteration.
- Allows you to test the free heap and PSRAM memory, ensuring that PSRAM is used properly during runtime.
- Ideal for memory-intensive applications that require more storage on ESP32 devices.

## Getting Started

### Prerequisites

1. **PlatformIO**: This example is intended to be used with PlatformIO. If you haven’t installed it yet, follow the [PlatformIO installation guide](https://platformio.org/install) to get started.
2. **ESP32 Board**: Make sure your ESP32 board supports PSRAM. For example, boards like the ESP32 DevKitC, ESP32-WROOM-32, and ESP32-WROVER boards come with PSRAM support.

### Hardware Requirements

- **ESP32 microcontroller** with PSRAM support.
- **PSRAM module** (if using an ESP32 without built-in PSRAM).

### Setup Instructions

1. **Clone the repository**:

   Clone the repository to your local machine:

   ```bash
   git clone https://github.com/PAT-IOT/stdpsram.git
