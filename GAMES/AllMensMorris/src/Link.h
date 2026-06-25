#pragma once

#include <Arduino.h>

#include "Rules.h"

enum LinkEventKind : uint8_t {
  LINK_EVENT_NONE,
  LINK_EVENT_START,
  LINK_EVENT_ACTION,
  LINK_EVENT_CURSOR,
  LINK_EVENT_MENU,
};

enum LinkStatus : uint8_t {
  LINK_STATUS_NONE,
  LINK_STATUS_I2C,
  LINK_STATUS_FLIPPED,
  LINK_STATUS_PROTOCOL,
};

struct LinkEvent {
  LinkEventKind kind = LINK_EVENT_NONE;
  uint8_t board = 0;
  Player firstPlayer = PLAYER_TWO;
  uint8_t ruleset = 0;
  TurnActionMode mode = TURN_ACTION_MOVE;
  uint8_t from = 255;
  uint8_t to = 255;
};

void linkBegin(uint32_t seed);
void linkUpdate(bool inMainMenu, uint8_t board, Player firstPlayer);
bool linkPeerAvailable();
LinkStatus linkStatus();
bool linkLocalIsPlayerOne();
Player linkLocalPlayer(Player firstPlayer);
bool linkConsumeEvent(LinkEvent &event);
void linkSendStart(uint8_t board, uint8_t ruleset, Player firstPlayer);
void linkSendAction(TurnActionMode mode, uint8_t from, uint8_t to);
void linkSendCursor(uint8_t point);
void linkSendMenu();
