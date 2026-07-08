[![Release](https://img.shields.io/github/v/release/sub1inear/ArduboyI2C?label=Release)](https://github.com/sub1inear/ArduboyI2C/releases/latest)
[![License](https://img.shields.io/github/license/sub1inear/ArduboyI2C?label=License)](https://github.com/sub1inear/ArduboyI2C/blob/main/LICENSE)
[![Arduino Library](https://img.shields.io/badge/Arduino-Library-00979D?logo=arduino)](https://docs.arduino.cc/libraries/arduboyi2c/)
[![PlatformIO Registry](https://img.shields.io/badge/PlatformIO-Registry-orange?logo=platformio)](https://registry.platformio.org/libraries/sub1inear/ArduboyI2C)
[![Docs](https://img.shields.io/badge/Docs-Online-blue)](https://sub1inear.github.io/ArduboyI2C/)

# ArduboyI2C Library
The **ArduboyI2C** library provides I2C support for Arduboy multiplayer games. It includes standard I2C functionality, support for multi-controller (master) setups, and helpers for multiplayer handshakes while keeping PROGMEM and RAM usage low.

## Documentation
See the [online documentation](https://sub1inear.github.io/ArduboyI2C/) for an API reference.
See [An I2C Primer](./docs/AnI2CPrimer.md) for a more in-depth explanation of writing I2C games.

## Features
- Controller (master) and target (slave) I2C support.
- Built-in handshake function for multiplayer games.
- Built-in cable flip detection for FX-C.
- Bufferless reading.
- Minimal memory overhead.
- Customizable configuration.

## Why ArduboyI2C over Wire?
- Highly optimized (saves ~1KiB of PROGMEM and ~200 bytes of RAM).
- Adds easy handshake functionality.
- Adds cable flip detection for FX-C.
- More customizable (see [Configuration](#configuration)).

## Installation

### Arduino IDE
Install via the Library Manager:
1. Open the Arduino IDE.
2. Go to **Sketch > Include Library > Manage Libraries...**
3. Search for "ArduboyI2C" and install the library by `sub1inear`.

### PlatformIO
Add to your `platformio.ini`:
```ini
lib_deps =
	sub1inear/ArduboyI2C
```

## Quick Start
### 1. Include the library
```cpp
#define I2C_IMPLEMENTATION
#include <ArduboyI2C.h>
```
### 2. Initialize
```cpp
void setup() {
    arduboy.begin();
    I2C::begin();
    ...
}
```
### 3. Handshake
```cpp
I2C::Role role = I2C::handshake();
```
### 4. Read/Write
```cpp
I2C::write(I2C::targetAddress, data, I2C::Mode::Sync);
I2C::read(I2C::targetAddress, data);
```
### 5. Callbacks
```cpp
I2C::onReceive(onReceive);
I2C::onRequest(onRequest);
```

```cpp
void onReceive() {
    const uint8_t *data = I2C::getBuffer();
    ...
}
void onRequest() {
    I2C::reply(data);
}
```

### 6. Follow these Rules:
1. One controller (master) and one or more targets (slaves) at all times.
2. Define `I2C_IMPLEMENTATION` in **one** source file before including `ArduboyI2C.h`.
3. In callbacks, use `I2C::reply()` and `I2C::getBuffer()`. Do not use `delay()`, `Serial`, `millis()`, etc.

## Configuration
Define before `#include <ArduboyI2C.h>`.
| Macro | Default | Options | Description |
|-------|---------|---------|-------------|
| `I2C_IMPLEMENTATION` | - | - | Must be defined in **one** source file to include the implementation. |
| `I2C_FREQUENCY` | 100000 | 1-200000 | Bus frequency (Hz) |
| `I2C_BUFFER_CAPACITY` | 32 | 1-255 | Transaction buffer size |
| `I2C_HANDSHAKE_BUSY_CHECKS` | 128 | 1-65535 | Number of times to check for a busy bus during a handshake. |
| `I2C_CHECK_CABLE_FLIPPED_CHECKS` | 128 | 1-65535 | Number of checks to perform during cable flip detection. |
| `I2C_CHECK_CABLE_FLIPPED_DEBOUNCE_CHECKS` | 128 | 1-65535 |  Number of passing flip checks before confirming cable is flipped back. |

## Migrating from Wire Library
| Wire API               | ArduboyI2C              |
| ---------------------- | ----------------------- |
| `Wire.begin()`         | `I2C::begin()`          |
| `Wire.begin(addr)`     | `I2C::setAddress(addr)` |
| `Wire.onReceive()`     | `I2C::onReceive()`      |
| `Wire.onRequest()`     | `I2C::onRequest()`      |
| `Wire.beginTransmission(addr)` -> `Wire.write(...)` -> `Wire.endTransmission()` | `I2C::write(address, data, mode)` |
| `Wire.requestFrom(address, quantity)` -> `Wire.read()` | `I2C::read(address, data, wait)` |
| `Wire.setClock()`      | `I2C_FREQUENCY`         |

## FAQ
### Why am I getting linker errors about ArduboyI2C functions?
Make sure you define `I2C_IMPLEMENTATION` in **one** source file before including `ArduboyI2C.h`.

### Why can't I use `delay()`, `Serial`, `millis()`, etc. inside `onReceive`/`onRequest` callbacks?
Callbacks are executed inside the I2C interrupt handler, where interrupts are disabled. Functions that rely on interrupts (`delay`, `millis`, `Serial`, etc.) won't work. Keep callbacks short and only use `I2C::reply()` and `I2C::getBuffer()`. If you need to do extended processing or rely on interrupts, set a `volatile bool` flag, copy the data out of the buffer, and then poll the flag in your main loop.

### Can I use ArduboyI2C in Ardens?
[Ardens](https://github.com/tiberiusbrown/Ardens) will soon support I2C multiplayer; stay tuned.

### When should I use `I2C::write()` vs `I2C::reply()`?
Use `I2C::write()` as the controller (master) to send data. Use `I2C::reply()` inside an `onRequest` callback to send data back when another device reads from you. Calling `write()` from a callback won't work correctly.

### How do I send/receive more than 32 bytes at once?
Writes and `reply()` are limited by `I2C_BUFFER_CAPACITY` (default: 32). Define it before including the header (e.g., `#define I2C_BUFFER_CAPACITY 64`). Reads have no such buffer limit.

### Why must `I2C::begin()` be called after `arduboy.begin()`?
`arduboy.boot()` (called inside `arduboy.begin()`) disables the TWI (I2C) hardware as a side effect. Calling `I2C::begin()` afterwards re-initializes it.

### Why can't I use certain I2C addresses for my own address?
Addresses 0–7 and 120–127 are reserved by the I2C spec (general call, etc.).

### I've passed my own loop function to `I2C::handshake()` or `I2C::checkCableFlipped()`, and it breaks handshaking/cable flip detection. Why?
The loop function disrupts the careful timing of the handshake/cable flip detection. You must experimentally redefine the `I2C_*_CHECKS` macros; see [Network of the Damned](./ports/NetworkOfTheDamned/Arduboy3D.ino) for an example of how to do this. Most likely, you want to increase `I2C_HANDSHAKE_BUSY_CHECKS` and `I2C_CHECK_CABLE_FLIPPED_CHECKS` while decreasing `I2C_CHECK_CABLE_FLIPPED_DEBOUNCE_CHECKS`.
