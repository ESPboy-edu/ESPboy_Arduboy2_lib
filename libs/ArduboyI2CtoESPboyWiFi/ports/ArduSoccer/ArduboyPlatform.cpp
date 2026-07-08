#define I2C_IMPLEMENTATION
// Убираем #define I2C_PLATFORM_FX_C, он нам больше не нужен
// --- CHANGED: Подключаем нашу библиотеку ---
#include <ArduboyI2CtoESPboyWiFi.h>
#include "ArduboyPlatform.h"
#include "Engine.h"

#define CENTER_STR(str, csize) (WIDTH / 2 - (sizeof(str) - 1) * csize / 2)

//#include "Generated/Data_Audio.h"

ArduboyPlatform Platform;

volatile bool controllerReceived = false;
volatile bool controllerRequested = false;

void onReceive()
{
    // --- CHANGED: ArduboyI2C namespace ---
  uint8_t *buffer = ArduboyI2C::getBuffer();
  Platform.lastInputState[REMOTE_PLAYER] = Platform.inputState[REMOTE_PLAYER];
  Platform.inputState[REMOTE_PLAYER] = buffer[0];
  controllerReceived = true;
}

void onRequest()
{
    // --- CHANGED: ArduboyI2C namespace ---
  ArduboyI2C::reply(Platform.inputState[LOCAL_PLAYER]);
  controllerRequested = true;
}

void ArduboyPlatform::updateInput()
{
  lastInputState[LOCAL_PLAYER] = inputState[LOCAL_PLAYER];
  inputState[LOCAL_PLAYER] = 0;

  if(arduboy.pressed(A_BUTTON))
  {
    inputState[LOCAL_PLAYER] |= Input_Btn_A;
  }
  if(arduboy.pressed(B_BUTTON))
  {
    inputState[LOCAL_PLAYER] |= Input_Btn_B;
  }
  if(arduboy.pressed(UP_BUTTON))
  {
    inputState[LOCAL_PLAYER] |= Input_Dpad_Up;
  }
  if(arduboy.pressed(DOWN_BUTTON))
  {
    inputState[LOCAL_PLAYER] |= Input_Dpad_Down;
  }
  if(arduboy.pressed(LEFT_BUTTON))
  {
    inputState[LOCAL_PLAYER] |= Input_Dpad_Left;
  }
  if(arduboy.pressed(RIGHT_BUTTON))
  {
    inputState[LOCAL_PLAYER] |= Input_Dpad_Right;
  }
}

void ArduboyPlatform::update()
{
    if(arduboy.audio.enabled() != !m_isMuted)
    {
    if(m_isMuted)
    {
      arduboy.audio.off();
    }
    else
    {
      arduboy.audio.on();
    }
    }
  updateInput();

  if (multiplayerConnected)
  {
      // --- ADDED: CRITICAL FIX FOR PASSIVE TARGET/SLAVE DEVICES ---
        // Force the network stack to process packets if we are the slave
        if (role == ArduboyI2C::Role::Target) {
            ArduboyI2C::update();
        }
        // -------------------------------------------------------------

    if (role == ArduboyI2C::Role::Controller)
    {
      do {
          // --- CHANGED: ArduboyI2C namespace, targetAddress, Mode::Sync ---
        ArduboyI2C::write(ArduboyI2C::targetAddress, inputState[LOCAL_PLAYER], ArduboyI2C::Mode::Sync);
      } while (ArduboyI2C::getError() != ArduboyI2C::Error::None);

      lastInputState[REMOTE_PLAYER] = inputState[REMOTE_PLAYER];
      do {
          // --- CHANGED: ArduboyI2C namespace ---
        ArduboyI2C::read(ArduboyI2C::targetAddress, inputState[REMOTE_PLAYER]);
      } while (ArduboyI2C::getError() != ArduboyI2C::Error::None);
    }
    else
    {
        // --- CHANGED: ArduboyI2C namespace ---
      ArduboyI2C::setAddress(ArduboyI2C::targetAddress);
      
      // --- ADDED: CRITICAL FIX ---
      // Feed the watchdog and process network while waiting!
      while (!controllerReceived) { ArduboyI2C::update(); }
      
      controllerReceived = false;
      
      // --- ADDED: CRITICAL FIX ---
      while (!controllerRequested) { ArduboyI2C::update(); }
      
      controllerRequested = false;
      ArduboyI2C::setAddress(ArduboyI2C::nullAddress);
    }
  }
}

void ArduboyPlatform::disconnectMultiplayer()
{
  multiplayerConnected = false;
    // --- ADDED: Выключаем сеть, если игрок отключился (экономим батарею) ---
    ArduboyI2C::end();
}

bool ArduboyPlatform::connectMultiplayer()
{
  arduboy.display(CLEAR_BUFFER);

    // --- ADDED: Инициализация сети ---
    // В ArduSoccer сеть лучше инициализировать прямо перед рукопожатием
    ArduboyI2C::begin();
    // ---------------------------------

  ArduboyI2C::checkCableFlipped([]() {
    engine.renderer.drawText(smallFont, PSTR("PLEASE FLIP THE CABLE"), CENTER_STR("PLEASE FLIP THE CABLE", 6), 30, 1);
    arduboy.display(CLEAR_BUFFER);
  });

  engine.renderer.drawText(smallFont, PSTR("WAITING FOR PLAYERS"), CENTER_STR("WAITING FOR PLAYERS", 6), 30, 1);
  arduboy.display(CLEAR_BUFFER);

    // --- CHANGED: Строгое сравнение ролей ---
  role = ArduboyI2C::handshake();
  
  if (role == ArduboyI2C::Role::Target)
  {
    ArduboyI2C::setAddress(ArduboyI2C::nullAddress);
    ArduboyI2C::onReceive(onReceive);
    ArduboyI2C::onRequest(onRequest);
  }

  multiplayerConnected = true;

  return role == ArduboyI2C::Role::Controller;
}

// Заглушка, если вдруг автор использует ее в других местах
void ERROR(const char* msg) {
    // В ESPboy можно перенаправить это в Serial или просто игнорировать
}
