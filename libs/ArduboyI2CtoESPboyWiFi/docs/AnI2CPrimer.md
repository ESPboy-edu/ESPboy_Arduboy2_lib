# An I2C Primer

## What Is I2C?

I2C (pronounced "I squared C", and sometimes written I²C or IIC) is a communication protocol that lets two or more devices talk to each other over two wires: one for data (SDA) and one for a clock signal (SCL). The Arduboy Mini and Arduboy FX-C both support I2C to connect multiple devices together.

I2C is a controller/target (often referred to as master/slave) protocol, meaning one device (the controller) initiates reads/writes, while another other device (the target) responds to the master's requests by providing or receiving data. Each target on the I2C bus has a unique address, which the controller uses to select which target to communicate with. Only one controller can be active at a time.


## Getting Started

Before anything else, you need to include `ArduboyI2C.h` in your sketch. You must define `I2C_IMPLEMENTATION` in **one** source file to include the implementation, otherwise the linker will complain about missing ArduboyI2C functions.

```cpp
#define I2C_IMPLEMENTATION
#include <ArduboyI2C.h>
```


Call `I2C::begin()` to initialize the I2C hardware.
This must be done after `arduboy.begin()` or `arduboy.boot()` because they both disable the I2C hardware to save power.

```cpp
void setup() {
    arduboy.begin();
    I2C::begin();
    ...
}
```

On the Arduboy FX-C, the link cable can be plugged in facing either direction.
`I2C::checkCableFlipped()` detects this and waits for the user to flip the cable back. You can pass two functions to it; one runs initially if the cable is flipped, and the other runs while the cable is still flipped; these both default to doing nothing if not provided. Here, we draw a message to the screen when the cable is first detected as flipped:

```cpp
I2C::checkCableFlipped([]() {
    drawMessage(F("Please flip the cable\non this device."));
});
```

`I2C::handshake()` waits for the other device and determines which one becomes the controller. It has the same API as `I2C::checkCableFlipped()`. It returns whether this device is the controller (master) or the target (slave). Here, we draw a message while waiting for the other device to respond:

```cpp
I2C::Role role = I2C::handshake([]() {
    drawMessage(F("Waiting for other\nplayer..."));
});
```


## State Synchronisation Multiplayer

State synchronisation multiplayer is the simplest multiplayer model: each frame, the controller sends its state to the target and reads the target's state back. Both devices update independently; they just keep each other informed about where things are.

This is how the Basic example works.
Each player has a position, and both devices exchange that position every frame:

```cpp
struct Player {
    int8_t x;
    int8_t y;
};

Player localPlayer  = { 0, 0 };
Player remotePlayer = { 0, 0 };
```

On the controller side, sending and receiving is straightforward:

```cpp
// read the remote player's position from the target
I2C::read(I2C::targetAddress, remotePlayer);
// send our position to the target (non-blocking)
I2C::write(I2C::targetAddress, localPlayer, I2C::Mode::Async);
```

`I2C::read()` and `I2C::write()` accept any object and automatically determine its size from its type.
The `I2C::Mode::Async` argument to `write()` makes it non-blocking; the library handles the transfer in the background using interrupts while your game logic continues.

On the target side, the controller is the one initiating everything.
The target registers callbacks to respond when the controller writes to it or reads from it:

```cpp
void onReceive() {
    remotePlayer = *reinterpret_cast<const Player *>(I2C::getBuffer());
}

void onRequest() {
    I2C::reply(localPlayer);
}

// in setup():
if (!isController) {
    I2C::onReceive(onReceive);
    I2C::onRequest(onRequest);
}
```

`onReceive` fires when the controller writes to the target.

The received bytes land in the internal buffer, which you access with `I2C::getBuffer()`. Within a callback, you can safely cast the returned pointer away from `const` to use it as scratch space.

`onRequest` fires when the controller reads from the target; use `I2C::reply()` to fill in what should be sent back.
`I2C::reply()` may only be called once per `onRequest` callback.

Both callbacks run inside an interrupt service routine, so they must be fast and must not use anything that depends on interrupts (`delay()`, `millis()`, `Serial`, etc.).

State synchronisation multiplayer is the best for games that have a small amount of game state;
with larger game state, lockstep multiplayer is the better option.

## Lockstep Multiplayer

In **lockstep multiplayer**, both devices run the exact same simulation from the same inputs, in the same frame order.
Neither device advances until both have exchanged inputs for the current frame.
This guarantees that both devices see exactly the same game state at all times — no prediction, no reconciliation, no drift.

This is how the [Pong example](./examples/Pong/Pong.ino) works.
The right player (controller) and left player (target) must both advance together, one frame at a time:

```cpp
// right player is the controller, left player is the target
bool isRightPlayer = false;
```

The controller sends its input first, then reads the target's input back.
It retries each operation until it succeeds:

```cpp
uint8_t rightInput = arduboy.buttonsState();
uint8_t leftInput;

// send our input and retry until acknowledged
do {
    I2C::write(I2C::targetAddress, rightInput, I2C::Mode::Sync);
} while (I2C::getError() != I2C::Error::None);

// read the target's input and retry until acknowledged
do {
    I2C::read(I2C::targetAddress, leftInput);
} while (I2C::getError() != I2C::Error::None);
```

The `I2C::Mode::Sync` in `write()` makes it blocking; the function does not return until the transfer has completed.
The `do`/`while` loop keeps retrying if the target is not yet ready, which is expected when the target needs a moment to set up for the new frame.

The target side is more involved.
It uses `volatile` variables to communicate between the main loop and the interrupt callbacks:

```cpp
volatile bool controllerReceived = false;
volatile uint8_t controllerInput  = 0;

volatile bool controllerRequested = false;
volatile uint8_t targetInput      = 0;

void onReceive() {
    controllerInput    = I2C::getBuffer()[0];
    controllerReceived = true;
}

void onRequest() {
    I2C::reply(targetInput);
    controllerRequested = true;
}
```

The `volatile` keyword is essential here.
Without it, the compiler may notice that `controllerReceived` is never written to from the main loop's point of view and assume it will never change, turning `while (!controllerReceived) { }` into an infinite loop.
`volatile` tells the compiler to always re-read the variable from memory, even if it does not appear to change.

The target then steps through a specific sequence each frame:

```cpp
// record our input before we start waiting
targetInput = arduboy.buttonsState();

// become visible on the bus — we are ready
I2C::setAddress(I2C::targetAddress);

// wait for the controller to send its input
while (!controllerReceived) { }
rightInput = controllerInput;
controllerReceived = false;

// wait for the controller to request our input
while (!controllerRequested) { }
controllerRequested = false;

// go invisible again — done for this frame
I2C::setAddress(I2C::nullAddress);
```

The `I2C::nullAddress` trick is the key to keeping both devices in sync.
When the target sets its address to `I2C::nullAddress`, it stops responding on the bus entirely.
This prevents the controller from accidentally reading ahead into the next frame and getting stale input before the target has had a chance to update it.
The target only becomes reachable once it is actually ready to participate in the new frame.

Both devices then call the same update function with the same inputs:

```cpp
update(leftInput, rightInput);
draw();
```

Because both devices run the exact same `update()` with the exact same `leftInput` and rightInput`, they will always produce exactly the same game state.

Lockstep multiplayer is the best for games that have a large amount of game state. However, it comes with a tradeoff: the game must be perfectly deterministic (no random numbers without a shared seed, etc.) and both devices must be perfectly in sync at all times, or the game will break.

## Sending Larger Data

Both `I2C::write()` and `I2C::reply()` use an internal buffer that holds 32 bytes by default.
If you need to transfer more than that at once, define `I2C_BUFFER_CAPACITY` before including the library:

```cpp
#define I2C_IMPLEMENTATION
#define I2C_BUFFER_CAPACITY 64
#include <ArduboyI2C.h>
```

When you need to send more than one field, pack everything into a struct and send the whole thing at once:

```cpp
struct GameState {
    int8_t  playerX;
    int8_t  playerY;
    uint8_t score;
    bool    attacking;
};

GameState localState;

// controller side
I2C::write(I2C::targetAddress, localState, I2C::Mode::Async);

// target side
void onRequest() {
    I2C::reply(localState);
}
```

The library deduces the size automatically.
On the receiving side, cast the raw buffer pointer back to the original type:

```cpp
void onReceive() {
    remoteState = *reinterpret_cast<const GameState *>(I2C::getBuffer());
}
```

## Error Handling

`I2C::getError()` returns the status of the last read or write:

- `I2C::Error::None`: no error.
- `I2C::Error::WriteAddrNack`: the target did not acknowledge during a write; it is not ready or not connected.
- `I2C::Error::ReadAddrNack`: the target did not acknowledge during a read; same cause.
- `I2C::Error::WriteDataNACK`: the target acknowledged its address but stopped responding during transfer.
- `I2C::Error::Bus`: an illegal start or stop condition was detected on the bus.

In a state-sync game you can often ignore errors and simply skip the update for that frame; the last known state will do.
In lockstep, you need to retry until success, as the Pong example shows.
