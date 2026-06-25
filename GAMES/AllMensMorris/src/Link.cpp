#include "Link.h"

#ifdef ALL_MENS_MORRIS_FXC_LINK
#define I2C_IMPLEMENTATION
#define I2C_BUFFER_SIZE 12
#define I2C_CHECK_BUS_BUSY_CHECKS 255
#include <ArduboyI2C.h>

namespace {
constexpr uint8_t LINK_MAGIC = 0xA9;
constexpr uint8_t LINK_VERSION = 1;
constexpr uint8_t LINK_KIND_BEACON = 1;
constexpr uint8_t LINK_KIND_START = 2;
constexpr uint8_t LINK_KIND_ACTION = 3;
constexpr uint8_t LINK_KIND_CURSOR = 4;
constexpr uint8_t LINK_KIND_MENU = 5;
constexpr uint8_t LINK_ADDRESS = 0x08;
constexpr uint8_t LINK_SEND_ADDRESS = 0x00;
constexpr uint8_t LINK_PEER_TIMEOUT_FRAMES = 120;
constexpr uint8_t LINK_SEND_INTERVAL_FRAMES = 45;
constexpr uint8_t LINK_SEND_JITTER_FRAMES = 45;
constexpr uint8_t LINK_TX_STUCK_FRAMES = 3;
constexpr uint8_t LINK_IDLE_LINE_CHECKS = 8;
#ifdef ALL_MENS_MORRIS_DEBUG
constexpr uint8_t LINK_LINE_SAMPLE_FRAMES = 6;
#endif

struct LinkPacket {
  uint8_t magic;
  uint8_t version;
  uint8_t kind;
  uint8_t seq;
  uint8_t nonce;
  uint8_t board;
  uint8_t firstPlayer;
  uint8_t mode;
  uint8_t from;
  uint8_t to;
};

volatile LinkPacket receivedPacket = {};
volatile bool hasReceivedPacket = false;

uint8_t localNonce = 1;
uint8_t sendFrame = 0;
uint8_t sendInterval = LINK_SEND_INTERVAL_FRAMES;
uint8_t sequence = 0;
uint8_t peerTimeout = 0;
uint8_t peerNonce = 0;
uint8_t lastEventSeq = 255;
bool peerAvailable = false;
bool pendingSendStart = false;
bool pendingSendAction = false;
bool pendingSendCursor = false;
bool linkStarted = false;
bool localStartedMatch = true;
uint8_t txActiveFrames = 0;
#ifdef ALL_MENS_MORRIS_DEBUG
uint8_t lineSampleFrame = 0;
LinkStatus diagnosticStatus = LINK_STATUS_NONE;
#endif
LinkPacket pendingAction = {};
LinkPacket pendingCursor = {};
LinkPacket pendingStart = {};
LinkEvent pendingEvent = {};

#if I2C_LIB_VER >= 30000
void onReceive(const uint8_t *buffer, uint8_t size);
#else
void onReceive();
#endif

void linkBeginBus() {
#if I2C_LIB_VER >= 30000
  I2C::begin();
#else
  I2C::init();
#endif
}

uint8_t linkError() {
#if I2C_LIB_VER >= 30000
  return I2C::error();
#else
  return I2C::getTWError();
#endif
}

void linkWritePacket(const LinkPacket &packet) {
#if I2C_LIB_VER >= 30000
  I2C::write(LINK_SEND_ADDRESS, packet, false);
#else
  I2C::write(LINK_SEND_ADDRESS, &packet, false);
#endif
}

bool linkLinesIdle() {
  for (uint8_t i = 0; i < LINK_IDLE_LINE_CHECKS; i++) {
    if (!(I2C_PIN & _BV(I2C_SDA_BIT)) || !(I2C_PIN & _BV(I2C_SCL_BIT))) {
      return false;
    }
    _delay_us(1000000.0 / I2C_FREQUENCY / 2.0);
  }
  return true;
}

void linkSetReceiveCallback() {
#if I2C_LIB_VER >= 30000
  I2C::onReceive(onReceive);
#else
  I2C::onReceive(onReceive);
#endif
}

void linkResetBus() {
  i2c_detail::data.active = false;
  linkBeginBus();
  I2C::setAddress(LINK_ADDRESS, true);
  linkSetReceiveCallback();
  txActiveFrames = 0;
}

#ifdef ALL_MENS_MORRIS_DEBUG
LinkStatus sampleLineStatus() {
#if I2C_LIB_VER >= 30000
  uint8_t sdaHigh = 0;
  uint8_t sclHigh = 0;
  for (uint8_t i = 0; i < I2C_CHECK_CABLE_FLIPPED_CHECKS; i++) {
    if (I2C_PIN & _BV(I2C_SDA_BIT)) {
      sdaHigh++;
    }
    if (I2C_PIN & _BV(I2C_SCL_BIT)) {
      sclHigh++;
    }
    _delay_us(1000000.0 / I2C_FREQUENCY / 2.0);
  }

  if (sdaHigh == I2C_CHECK_CABLE_FLIPPED_CHECKS
      && sclHigh == I2C_CHECK_CABLE_FLIPPED_CHECKS) {
    return LINK_STATUS_NONE;
  }

  constexpr uint8_t halfChecks = I2C_CHECK_CABLE_FLIPPED_CHECKS / 2;
  uint8_t sdaScore = static_cast<uint8_t>(abs(static_cast<int8_t>(sdaHigh - halfChecks)));
  uint8_t sclScore = static_cast<uint8_t>(abs(static_cast<int8_t>(sclHigh - halfChecks)));
  return sdaScore < sclScore ? LINK_STATUS_FLIPPED : LINK_STATUS_I2C;
#else
  return LINK_STATUS_NONE;
#endif
}
#endif

bool linkBusReady() {
  if (!linkStarted) {
    return false;
  }
  if (!i2c_detail::data.active) {
    txActiveFrames = 0;
    return true;
  }
  if (++txActiveFrames >= LINK_TX_STUCK_FRAMES) {
    linkResetBus();
  }
  return false;
}

void receivePacket(const uint8_t *buffer, uint8_t size) {
  if (size < sizeof(LinkPacket)) {
    return;
  }
  const LinkPacket *packet = reinterpret_cast<const LinkPacket *>(buffer);
  if (packet->magic != LINK_MAGIC || packet->version != LINK_VERSION) {
    return;
  }
  receivedPacket = *packet;
  hasReceivedPacket = true;
}

#if I2C_LIB_VER >= 30000
void onReceive(const uint8_t *buffer, uint8_t size) {
  receivePacket(buffer, size);
}
#else
void onReceive() {
  receivePacket(I2C::getBuffer(), sizeof(LinkPacket));
}
#endif

bool sendPacket(LinkPacket packet) {
  if (!linkBusReady()) {
    return false;
  }
  if (!linkLinesIdle()) {
    return false;
  }
  packet.magic = LINK_MAGIC;
  packet.version = LINK_VERSION;
  packet.seq = sequence++;
  packet.nonce = localNonce;
  linkWritePacket(packet);
  if (linkError() != TW_SUCCESS) {
    i2c_detail::data.active = false;
    return false;
  }
  return true;
}

uint8_t nextSendInterval() {
  return LINK_SEND_INTERVAL_FRAMES + random(LINK_SEND_JITTER_FRAMES + 1);
}

void queueEvent(const LinkPacket &packet) {
  if (packet.seq == lastEventSeq) {
    return;
  }
  lastEventSeq = packet.seq;
  pendingEvent.board = packet.board;
  pendingEvent.firstPlayer = static_cast<Player>(packet.firstPlayer);
  pendingEvent.ruleset = packet.mode;
  pendingEvent.mode = static_cast<TurnActionMode>(packet.mode);
  pendingEvent.from = packet.from;
  pendingEvent.to = packet.to;
  if (packet.kind == LINK_KIND_START) {
    localStartedMatch = false;
    pendingEvent.kind = LINK_EVENT_START;
  } else if (packet.kind == LINK_KIND_ACTION) {
    pendingEvent.kind = LINK_EVENT_ACTION;
  } else if (packet.kind == LINK_KIND_CURSOR) {
    pendingEvent.kind = LINK_EVENT_CURSOR;
  } else if (packet.kind == LINK_KIND_MENU) {
    pendingEvent.kind = LINK_EVENT_MENU;
  }
}

void processReceived() {
  if (!hasReceivedPacket) {
    return;
  }
  noInterrupts();
  LinkPacket packet = {};
  packet.magic = receivedPacket.magic;
  packet.version = receivedPacket.version;
  packet.kind = receivedPacket.kind;
  packet.seq = receivedPacket.seq;
  packet.nonce = receivedPacket.nonce;
  packet.board = receivedPacket.board;
  packet.firstPlayer = receivedPacket.firstPlayer;
  packet.mode = receivedPacket.mode;
  packet.from = receivedPacket.from;
  packet.to = receivedPacket.to;
  hasReceivedPacket = false;
  interrupts();

  if (packet.nonce == localNonce) {
    return;
  }
  peerAvailable = true;
  peerTimeout = LINK_PEER_TIMEOUT_FRAMES;
  peerNonce = packet.nonce;
  if (packet.kind == LINK_KIND_START
      || packet.kind == LINK_KIND_ACTION
      || packet.kind == LINK_KIND_CURSOR
      || packet.kind == LINK_KIND_MENU) {
    queueEvent(packet);
  }
}

void sendBeacon(uint8_t board, Player firstPlayer) {
  LinkPacket packet = {};
  packet.kind = LINK_KIND_BEACON;
  packet.board = board;
  packet.firstPlayer = firstPlayer;
  packet.mode = TURN_ACTION_MOVE;
  packet.from = 255;
  packet.to = 255;
  sendPacket(packet);
}
}

void linkBegin(uint32_t seed) {
  localNonce = static_cast<uint8_t>((seed ^ (seed >> 8) ^ (seed >> 16)) & 0x7F);
  if (localNonce == 0) {
    localNonce = 1;
  }
  sendFrame = localNonce % LINK_SEND_INTERVAL_FRAMES;
  sendInterval = nextSendInterval();
  linkStarted = true;
  linkResetBus();
}

void linkUpdate(bool inMainMenu, uint8_t board, Player firstPlayer) {
  linkBusReady();
  processReceived();
  if (peerTimeout > 0) {
    peerTimeout--;
#ifdef ALL_MENS_MORRIS_DEBUG
    diagnosticStatus = LINK_STATUS_PROTOCOL;
#endif
  } else {
    peerAvailable = false;
#ifdef ALL_MENS_MORRIS_DEBUG
    if (inMainMenu && !i2c_detail::data.active && lineSampleFrame++ >= LINK_LINE_SAMPLE_FRAMES) {
      lineSampleFrame = 0;
      diagnosticStatus = sampleLineStatus();
    }
#endif
  }

  if (pendingSendStart) {
    pendingSendStart = false;
    if (!sendPacket(pendingStart)) {
      pendingSendStart = true;
    }
    return;
  }
  if (pendingSendCursor) {
    pendingSendCursor = false;
    if (!sendPacket(pendingCursor)) {
      pendingSendCursor = true;
    }
    return;
  }
  if (pendingSendAction) {
    pendingSendAction = false;
    if (!sendPacket(pendingAction)) {
      pendingSendAction = true;
    }
    return;
  }
  if (inMainMenu && sendFrame++ >= sendInterval) {
    sendFrame = 0;
    sendInterval = nextSendInterval();
    sendBeacon(board, firstPlayer);
  }
}

bool linkPeerAvailable() {
  return peerAvailable;
}

LinkStatus linkStatus() {
#ifdef ALL_MENS_MORRIS_DEBUG
  return peerAvailable ? LINK_STATUS_PROTOCOL : diagnosticStatus;
#else
  return peerAvailable ? LINK_STATUS_PROTOCOL : LINK_STATUS_NONE;
#endif
}

bool linkLocalIsPlayerOne() {
  return localStartedMatch;
}

Player linkLocalPlayer(Player firstPlayer) {
  return linkLocalIsPlayerOne() ? firstPlayer : opponentOf(firstPlayer);
}

bool linkConsumeEvent(LinkEvent &event) {
  if (pendingEvent.kind == LINK_EVENT_NONE) {
    return false;
  }
  event = pendingEvent;
  pendingEvent.kind = LINK_EVENT_NONE;
  return true;
}

void linkSendStart(uint8_t board, uint8_t ruleset, Player firstPlayer) {
  pendingStart = {};
  pendingStart.kind = LINK_KIND_START;
  pendingStart.board = board;
  pendingStart.firstPlayer = firstPlayer;
  pendingStart.mode = static_cast<TurnActionMode>(ruleset);
  pendingStart.from = 255;
  pendingStart.to = 255;
  localStartedMatch = true;
  pendingSendStart = true;
}

void linkSendAction(TurnActionMode mode, uint8_t from, uint8_t to) {
  pendingAction = {};
  pendingAction.kind = LINK_KIND_ACTION;
  pendingAction.mode = mode;
  pendingAction.from = from;
  pendingAction.to = to;
  pendingSendAction = true;
}

void linkSendCursor(uint8_t point) {
  pendingCursor = {};
  pendingCursor.kind = LINK_KIND_CURSOR;
  pendingCursor.to = point;
  pendingCursor.from = 255;
  pendingCursor.mode = TURN_ACTION_MOVE;
  pendingSendCursor = true;
}

void linkSendMenu() {
  pendingAction = {};
  pendingAction.kind = LINK_KIND_MENU;
  pendingAction.from = 255;
  pendingAction.to = 255;
  pendingAction.mode = TURN_ACTION_MOVE;
  pendingSendAction = true;
}

#else

void linkBegin(uint32_t) {}
void linkUpdate(bool, uint8_t, Player) {}
bool linkPeerAvailable() { return false; }
LinkStatus linkStatus() { return LINK_STATUS_NONE; }
bool linkLocalIsPlayerOne() { return true; }
Player linkLocalPlayer(Player firstPlayer) { return firstPlayer; }
bool linkConsumeEvent(LinkEvent &) { return false; }
void linkSendStart(uint8_t, uint8_t, Player) {}
void linkSendAction(TurnActionMode, uint8_t, uint8_t) {}
void linkSendCursor(uint8_t) {}
void linkSendMenu() {}

#endif
