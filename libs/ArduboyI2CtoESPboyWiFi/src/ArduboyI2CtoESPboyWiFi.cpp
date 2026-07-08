#include "ArduboyI2CtoESPboyWiFi.h"
#include <Arduboy2.h> // Include standard graphics for error messages and native LED control

// Low-level ESP8266 API to disable system Serial spam
extern "C" {
  #include "user_interface.h"
}

// Link the library to the global console object created inside the game (.ino)
extern Arduboy2 arduboy; 

// v3.0.0: Static constants definition
const uint8_t ArduboyI2C::targetAddress;
const uint8_t ArduboyI2C::nullAddress;

// Static variables initialization
uint8_t ArduboyI2C::peerMac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
bool ArduboyI2C::connected = false;
bool ArduboyI2C::isMaster = false;
unsigned long ArduboyI2C::lastHeartbeat = 0;
uint8_t ArduboyI2C::connState = 0; // 0 = Searching, 1 = Connected, 2 = Lag/Lost connection

uint8_t ArduboyI2C::txBuffer[250];
size_t ArduboyI2C::txLen = 0;

uint8_t ArduboyI2C::rxBuffer[512];
uint16_t ArduboyI2C::rxHead = 0;
uint16_t ArduboyI2C::rxTail = 0;
uint8_t ArduboyI2C::rxFlatBuffer[250];

uint8_t ArduboyI2C::gameHash[4] = {0, 0, 0, 0};

bool ArduboyI2C::gameStarted = false;
unsigned long ArduboyI2C::gameStartTime = 0;
bool ArduboyI2C::syncLost = false;

void (*ArduboyI2C::userOnReceive)() = nullptr;
void (*ArduboyI2C::userOnRequest)() = nullptr;

#ifdef ENABLE_WIFI_LED_INDICATION
void ArduboyI2C::updateLEDState() {
    if (connState == 0) {
        // Dim Yellow: Searching for peer in the air
        arduboy.setRGBled(5, 5, 0);
    } else if (connState == 1) {
        // Dim Green: Stable connection established
        arduboy.setRGBled(0, 5, 0);
    } else if (connState == 2) {
        // Dim Red: Packet loss, lag, or peer disconnected
        arduboy.setRGBled(5, 0, 0);
    }
}
#endif

void ArduboyI2C::begin() {
    // Hard disable any output from the closed ESP8266 core to Serial
    system_set_os_print(0); 

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    if (esp_now_init() != 0) {
        // Keep only our critical error message
        Serial.println("Error initializing ESP-NOW"); 
        return;
    }

    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);

    uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);

    connState = 0; 
    gameStarted = false;
    syncLost = false;

    // Generate a unique identifier for the current sketch/game using its MD5 hash
    String md5 = ESP.getSketchMD5();
    for (int i = 0; i < 4; i++) {
        gameHash[i] = md5[i];
    }

#ifdef ENABLE_WIFI_LED_INDICATION
    updateLEDState(); 
#endif
}

void ArduboyI2C::end() {
    if (connected) {
        WiFi.disconnect();
        connected = false;
        connState = 0;
        #ifdef ENABLE_WIFI_LED_INDICATION
        updateLEDState();
        #endif
    }
}

void ArduboyI2C::update() {
    // Allow the ESP8266 OS to service background Wi-Fi tasks
    yield();
    // Hardware Watchdog reset to prevent cyclic chip reboot
    ESP.wdtFeed();

    // --- FATAL DESYNC HANDLING ---
    if (syncLost) {
        arduboy.clear();
        arduboy.setCursor(10, 20); 
        arduboy.print(F("Synch lost,\nrestarting..."));
        arduboy.display();

        // Safe 3-second delay with continuous Watchdog feeding
        unsigned long waitStart = millis();
        while (millis() - waitStart < 3000) {
            yield();
            ESP.wdtFeed();
        }
        
        // Soft restart the console to the game menu for a clean sync
        ESP.restart(); 
    }

    unsigned long currentMillis = millis();

    if (!connected) {
        // Broadcast a DISCOVER packet every 500ms containing our game hash
        static unsigned long lastDiscover = 0;
        if (currentMillis - lastDiscover > 500) {
            lastDiscover = currentMillis;
            uint8_t msg[5] = {PKT_DISCOVER, gameHash[0], gameHash[1], gameHash[2], gameHash[3]};
            uint8_t broadcast[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
            esp_now_send(broadcast, msg, 5);
        }
    } else {
        // If connected, send a Ping once a second to show we are alive
        static unsigned long lastPing = 0;
        if (currentMillis - lastPing > 1000) {
            lastPing = currentMillis;
            uint8_t msg = PKT_PING;
            esp_now_send(peerMac, &msg, 1);
        }

        // --- NETWORK WATCHDOG (TIMEOUTS) ---
        if (currentMillis - lastHeartbeat > 2000) {
            // If silence lasts > 2 seconds, turn on warning red LED
            if (connState != 2) {
                connState = 2; 
                #ifdef ENABLE_WIFI_LED_INDICATION
                updateLEDState();
                #endif
            }
            
            // If silence lasts > 5 seconds, connection is considered dead
            if (currentMillis - lastHeartbeat > 5000) {
                if (gameStarted) {
                    // If the match was ongoing, trigger the error screen and reboot
                    syncLost = true; 
                } else {
                    // If we were stuck in the menu, silently drop session and search again
                    connected = false;
                    connState = 0;
                    #ifdef ENABLE_WIFI_LED_INDICATION
                    updateLEDState();
                    #endif
                }
            }
        } else if (connState == 2) {
            // Connection restored within 5 seconds - return to green
            connState = 1; 
            #ifdef ENABLE_WIFI_LED_INDICATION
            updateLEDState();
            #endif
        }
    }
}

bool ArduboyI2C::isConnected() {
    return connected;
}

uint8_t ArduboyI2C::getPlayerID() {
    return isMaster ? 0 : 1;
}

void ArduboyI2C::beginTransmission() {
    txLen = 0;
}

void ArduboyI2C::write(uint8_t data) {
    if (txLen < sizeof(txBuffer)) {
        txBuffer[txLen++] = data;
    }
}

void ArduboyI2C::write(uint8_t* data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        write(data[i]);
    }
}

// v3.0.0 raw write override
void ArduboyI2C::write(uint8_t address, const void* buffer, uint8_t size, Mode mode) {
    if (!connected || size == 0) return;
    beginTransmission();
    write((uint8_t*)buffer, size);
    endTransmission();
}

// v3.0.0 raw read override
void ArduboyI2C::read(uint8_t address, void* buffer, uint8_t size) {
    if (!connected || size == 0) return;
    rxTail = rxHead;
    uint8_t req = PKT_REQUEST;
    esp_now_send(peerMac, &req, 1);

    unsigned long start = millis();
    unsigned long lastReq = millis();
    
    while (available() < (int)size) {
        if (!connected) return;
        if (millis() - start > 500) return;
        if (millis() - lastReq > 200) {
            esp_now_send(peerMac, &req, 1);
            lastReq = millis();
        }
        // update() is legal here because this is a safe waiting loop 
        // specifically meant to block until data arrives
        update(); 
    }

    uint8_t* ptr = (uint8_t*)buffer;
    for (uint8_t i = 0; i < size; i++) {
        ptr[i] = ArduboyI2C::read();
    }
}

// v3.0.0 raw reply override
void ArduboyI2C::reply(const void* buffer, uint8_t size) {
    beginTransmission();
    write((uint8_t*)buffer, size);
    endTransmission();
}

void ArduboyI2C::endTransmission() {
    if (!connected || txLen == 0) return;

    uint8_t packet[250];
    packet[0] = PKT_DATA;
    memcpy(&packet[1], txBuffer, txLen);
    
    esp_now_send(peerMac, packet, txLen + 1);
    txLen = 0;
}

int ArduboyI2C::available() {
    return (rxHead - rxTail) & 511;
}

int ArduboyI2C::read() {
    if (rxHead == rxTail) return -1;
    uint8_t data = rxBuffer[rxTail];
    rxTail = (rxTail + 1) & 511;
    return data;
}

uint8_t* ArduboyI2C::getBuffer() {
    return rxFlatBuffer;
}

void ArduboyI2C::onReceive(void (*callback)()) {
    userOnReceive = callback;
}

void ArduboyI2C::onRequest(void (*callback)()) {
    userOnRequest = callback;
}

void ArduboyI2C::OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
}

void ArduboyI2C::OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
    if (len == 0) return;

    uint8_t type = incomingData[0];

    if (connected && memcmp(mac, peerMac, 6) == 0) {
        lastHeartbeat = millis();
    }

    if (type == PKT_DISCOVER && len == 5) {
        if (memcmp(&incomingData[1], gameHash, 4) != 0) {
            return; 
        }

        if (gameStarted && (millis() - gameStartTime > 1500)) {
            syncLost = true; 
            return; 
        }

        if (!connected) {
            memcpy(peerMac, mac, 6);
            esp_now_add_peer(peerMac, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
            
            connected = true;
            connState = 1; 
            lastHeartbeat = millis();

            uint8_t myMac[6];
            WiFi.macAddress(myMac);
            isMaster = memcmp(myMac, peerMac, 6) < 0; 

#ifdef ENABLE_WIFI_LED_INDICATION
            updateLEDState();
#endif
            uint8_t msg[5] = {PKT_DISCOVER, gameHash[0], gameHash[1], gameHash[2], gameHash[3]};
            esp_now_send(mac, msg, 5);
        }
    } 
    else if (type == PKT_DATA && connected) {
        int dataLen = len - 1;
        memcpy(rxFlatBuffer, &incomingData[1], dataLen);

        for (int i = 1; i < len; i++) {
            rxBuffer[rxHead] = incomingData[i];
            rxHead = (rxHead + 1) & 511;
            
            if (rxHead == rxTail) {
                rxTail = (rxTail + 1) & 511; 
            }
        }

        if (userOnReceive) {
            userOnReceive();
        }
    }
    else if (type == PKT_REQUEST && connected) {
        if (userOnRequest) {
            userOnRequest();
        }
    }
}