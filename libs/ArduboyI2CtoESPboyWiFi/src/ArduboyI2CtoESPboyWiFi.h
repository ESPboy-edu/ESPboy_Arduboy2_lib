#ifndef ARDUBOY_I2C_ESPNOW_H
#define ARDUBOY_I2C_ESPNOW_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

// Uncomment the line below to enable ESPboy RGB LED WiFi state indication
// We use native Arduboy2 LED methods, so no external libraries are needed!
#define ENABLE_WIFI_LED_INDICATION

// --- LEGACY MACROS (For backward compatibility with older games) ---
#define I2C_TARGET_ADDRESS 0x08
#define I2C_NULL_ADDRESS   0x09
#define I2C_ERROR_NONE     0x00

// --- ESP-NOW PROTOCOL PACKET TYPES ---
#define PKT_DISCOVER 0x00
#define PKT_DATA     0x01
#define PKT_PING     0x02
#define PKT_REQUEST  0x03

class ArduboyI2C {
public:
    // --- V3.0.0 ENUMS ---
    enum class Mode : uint8_t { Async, Sync };
    enum class Error : uint8_t { 
        WriteAddrNack = 0x20, 
        WriteDataNack = 0x30, 
        ReadAddrNack = 0x48, 
        Bus = 0x00, 
        None = 0xFF 
    };
    enum class Role : uint8_t { Controller, Target };

    // --- V3.0.0 CLASS CONSTANTS ---
    static const uint8_t targetAddress = 0x08;
    static const uint8_t nullAddress = 0x09;

    // Initialization and connection management
    static void begin();
    static void end();
    static void update();
    static bool isConnected();
    static uint8_t getPlayerID();
    
    // v3.0.0: Returns Role enum and accepts optional callback functions
    template <typename StartFunc = void(*)(), typename LoopFunc = void(*)()>
    static Role handshake(StartFunc startFunction = nullptr, LoopFunc loopFunction = nullptr);

    // --- DUMMY METHODS FOR BACKWARD COMPATIBILITY ---
    
    // v3.0.0: Accepts optional start/loop callbacks
    template <typename StartFunc = void(*)(), typename LoopFunc = void(*)()>
    static void checkCableFlipped(StartFunc startFunction = nullptr, LoopFunc loopFunction = nullptr) { }

    static void setAddress(uint8_t address) { }
    
    // v3.0.0: New dummy state tracking methods
    static bool isActive() { return false; } 
    static uint8_t getAddress() { return isMaster ? 0 : targetAddress; }

    // v3.0.0: Returns Error enum instead of uint8_t
    static Error getError() { 
        update(); 
        return connected ? Error::None : Error::Bus; 
    }

    // ------------------------------------------------

    // Standard raw data transmission methods
    static void beginTransmission();
    static void write(uint8_t data);
    static void write(uint8_t* data, size_t length); 
    static void write(uint8_t address, const void* buffer, uint8_t size, Mode mode = Mode::Sync);
    static void read(uint8_t address, void* buffer, uint8_t size);
    static void reply(const void* buffer, uint8_t size);
    static void endTransmission();

    // v3.0.0: Overloaded template write 
    template <typename T>
    static void write(uint8_t address, const T& data, Mode mode = Mode::Sync) {
        if (!connected) return;
        ArduboyI2C::write(address, (const void*)&data, sizeof(T), mode);
    }

    // v3.0.0: Template reply overload 
    template <typename T>
    static void reply(const T& data) {
        ArduboyI2C::reply((const void*)&data, sizeof(T));
    }

    // v3.0.0: Template read overload 
    template <typename T>
    static void read(uint8_t address, T& data) {
        ArduboyI2C::read(address, (void*)&data, sizeof(T));
    }

    // Data reception stream methods
    static int available();
    static int read();
    static uint8_t* getBuffer();
    
    // v3.0.0: New alias function for available()
    static size_t getBufferSize() { return available(); }
    
    // Callbacks
    static void onReceive(void (*callback)());
    static void onRequest(void (*callback)());

private:
    static uint8_t peerMac[6];
    static bool connected;
    static bool isMaster;
    
    // Buffers
    static uint8_t txBuffer[250];
    static size_t txLen;
    static uint8_t rxBuffer[512]; 
    static uint16_t rxHead;
    static uint16_t rxTail;
    static uint8_t rxFlatBuffer[250];

    // Callback pointers
    static void (*userOnReceive)();
    static void (*userOnRequest)();

    // Internal ESP-NOW callbacks
    static void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus);
    static void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len);

    // Heartbeat tracking
    static unsigned long lastHeartbeat;
    static uint8_t connState; 
    
    // Unique Game Identifier (based on sketch MD5 hash)
    static uint8_t gameHash[4];

    // Synchronization control
    static bool gameStarted;
    static unsigned long gameStartTime;
    static bool syncLost;

#ifdef ENABLE_WIFI_LED_INDICATION
    static void updateLEDState();
#endif
};

// Handshake template implementation inside the header to satisfy the linker
template <typename StartFunc, typename LoopFunc>
ArduboyI2C::Role ArduboyI2C::handshake(StartFunc startFunction, LoopFunc loopFunction) {
    if (startFunction) {
        startFunction();
    }
    while (!connected) {
        update();
        delay(10);
        
        // Execute the optional loop function if provided (e.g., to render "Waiting...")
        if (loopFunction) {
            loopFunction();
        }
    }
    
    gameStarted = true;
    gameStartTime = millis();
    
    // Return v3.0.0 Role enum
    return isMaster ? Role::Controller : Role::Target;
}

#endif