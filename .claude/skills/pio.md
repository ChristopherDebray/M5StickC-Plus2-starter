# Skill: pio

PlatformIO build / upload / monitor shortcuts for this project.

## Trigger

User says: "build", "compile", "upload", "flash", "monitor", "serial", "/pio [action]"

## Commands

| Intent | Command |
|--------|---------|
| Build only | `pio run` |
| Upload | `pio run -t upload` |
| Serial monitor | `pio device monitor --baud 115200` |
| Upload + monitor | `pio run -t upload && pio device monitor --baud 115200` |
| Clean build | `pio run -t clean && pio run` |
| Native unit tests | `pio test -e native` |
| On-device tests | `pio test -e m5stick-c` |
| List devices | `pio device list` |

## Environments

- `m5stick-c` — target device (ESP32, M5StickC-Plus2)
- `native` — host-side unit tests (no device needed, C++11)

## Notes
- Monitor baud is 115200 (set in `platformio.ini`)
- `CORE_DEBUG_LEVEL=0` in build_flags — increase to 5 for verbose ESP-IDF logs
- Optional libs (AsyncMqttClient, ArduinoJson) already in `lib_deps` — remove if unused to speed up builds
