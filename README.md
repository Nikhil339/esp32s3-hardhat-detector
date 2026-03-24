# Hardhat Detection on ESP32-S3 (ESP-DL)

This project implements a lightweight, high-performance hardhat detection system for industrial safety, running natively on the **ESP32-S3** using the [ESP-DL](https://github.com/espressif/esp-dl) deep learning framework.

## 🚀 Overview

The system loads a pre-compiled deep learning model (`.espdl`) directly from Flash memory into the ESP32-S3's internal accelerators. It includes a built-in profiling engine to measure layer-by-layer latency, memory consumption, and mathematical accuracy against the original Python training results.

### Key Features
- **Flash-Native Execution:** Uses `fbs::MODEL_LOCATION_IN_FLASH_RODATA` to minimize RAM footprint.
- **Hardware Optimized:** Leverages the ESP32-S3's AI instructions and Octal SPIRAM for maximum throughput.
- **Automated Validation:** Includes a C++ math test suite that compares hardware output directly against reference values.
- **Performance Profiling:** Detailed reporting of layer speeds and peak memory usage.

## 🛠 Hardware Requirements

To achieve the best performance, this project is configured for:
- **SoC:** ESP32-S3 (N8R8 or similar)
- **Memory:** 8MB Octal SPIRAM (PSRAM)
- **Flash:** 8MB (minimum)

## 🏗 Project Structure

```text
├── components/camera/      # Interface for ESP32-Camera integration
├── main/
│   ├── main.cpp            # Entry point, profiling & validation logic
│   ├── hardhat_model.espdl # Compiled deep learning model binary
│   └── CMakeLists.txt      # Handles binary model embedding
├── partitions.csv          # Custom 8MB flash partition table
└── sdkconfig.defaults      # Pre-configured PSRAM and optimization settings
```

## 🚦 Getting Started

### 1. Prerequisites
- [ESP-IDF v4.4+](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)
- ESP-DL Component (automatically handled by `idf_component.yml`)

### 2. Build & Flash
```bash
# Set target to ESP32-S3
idf.py set-target esp32s3

# Build the project
idf.py build

# Flash and monitor output
idf.py -p <PORT> flash monitor
```

## 📊 Performance Analysis

The application automatically outputs a detailed performance report to the serial monitor:

- **Math Validation:** Confirms if the embedded model's C++ execution matches the Python training results.
- **Memory Profiling:** Tracks peak RAM usage and buffer allocations in PSRAM.
- **Layer Latency:** Lists the execution time of every model module, sorted from slowest to fastest to help identify bottlenecks.

## 🗺 Roadmap
- [ ] **Camera Integration:** Stream live frames from an OV2640/OV5640 sensor.
- [ ] **Inference Logic:** Implement bounding box decoding and NMS (Non-Maximum Suppression).
- [ ] **Alert System:** Trigger GPIO pins or MQTT messages when a worker without a hardhat is detected.

## 📄 License
This project is licensed under the Apache 2.0 License. See the [LICENSE](LICENSE) file for details.
