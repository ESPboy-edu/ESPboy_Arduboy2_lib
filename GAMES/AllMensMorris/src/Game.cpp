#include "AllMensMorrisGame.h"

#include "Ai.h"
#include "Assets.h"
#include "Board.h"
#include "GeneratedBoards.h"
#include "Link.h"
#include "MenuMusic.h"
#include "Rules.h"

#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <Tinyfont.h>

namespace {
Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, Arduboy2::width(), Arduboy2::height());

constexpr uint8_t GAME_FPS = 30;
constexpr uint8_t BASE_FPS = 10;
constexpr uint8_t FPS_SCALE = GAME_FPS / BASE_FPS;
constexpr uint8_t framesAtGameFps(uint8_t baseFrames) {
  return baseFrames * FPS_SCALE;
}
constexpr uint8_t BOARD_OFFSET_X = 32;
constexpr uint8_t BOARD_OFFSET_Y = 0;
constexpr uint8_t HUD_LEFT_X = 0;
constexpr uint8_t HUD_RIGHT_X = 100;
constexpr uint8_t NO_POINT = 255;
constexpr uint8_t HUD_PIECE_WIDTH = 16;
constexpr uint8_t HUD_PIECE_HEIGHT = 10;
constexpr uint8_t HUD_STACK_STEP_Y = 3;
constexpr uint8_t HUD_STACK_BOTTOM_Y = 60;
constexpr uint8_t HUD_STACK_SINGLE_BOTTOM_Y = 45;
constexpr uint8_t HISTORY_CAPACITY = 8;
constexpr uint8_t MENU_ITEM_COUNT = 4;
constexpr uint8_t BOARD_MENU_COUNT = MORRIS_BOARD_PROFILE_COUNT;
constexpr uint16_t MENU_IDLE_DEMO_FRAMES = GAME_FPS * 20;
constexpr uint16_t DEMO_TOTAL_FRAMES = GAME_FPS * 30;
constexpr uint8_t DEMO_WIN_EXTRA_FRAMES = GAME_FPS * 3;
constexpr uint8_t CURTAIN_FRAMES = 20;
constexpr uint8_t DEMO_MIN_PREFLIGHT_ACTIONS = 10;
constexpr uint8_t DEMO_PREFLIGHT_ACTION_RANGE = 10;
constexpr uint8_t DEMO_CPU_MIN_THINK_FRAMES = 6;
constexpr uint8_t DEMO_CPU_MAX_THINK_FRAMES = 24;
constexpr uint8_t CPU_THINK_FRAMES = 14;
constexpr uint8_t CPU_ANIMATION_TARGET_FRAMES = 30;
constexpr uint8_t CPU_ACTION_PAUSE_FRAMES = 3;
constexpr uint8_t CPU_MIN_STEP_FRAMES = 2;
constexpr uint8_t CPU_MAX_STEP_FRAMES = 7;
constexpr uint8_t CURSOR_CHORD_FRAMES = 1;
constexpr uint8_t BOOT_LOGO_X = 52;
constexpr uint8_t BOOT_LOGO_Y = 26;
constexpr uint8_t BOOT_DUST_START_FRAMES = framesAtGameFps(20);
constexpr uint8_t BOOT_FILL_START_FRAMES = framesAtGameFps(32);
constexpr uint8_t BOOT_CURTAIN_START_FRAMES = framesAtGameFps(47);
constexpr uint8_t BOOT_TOTAL_FRAMES = framesAtGameFps(52);
constexpr uint8_t MENU_MUSIC_RESTART_FRAMES = 12;
constexpr uint8_t MENU_MUSIC_INITIAL_DELAY_FRAMES = GAME_FPS;
constexpr uint8_t AUDIO_OSD_FRAMES = GAME_FPS;
constexpr uint8_t BOARD_WHEEL_X = 31;
constexpr uint8_t BOARD_WHEEL_Y = 25;
constexpr uint8_t BOARD_WHEEL_W = 66;
constexpr uint8_t BOARD_WHEEL_H = 11;
constexpr uint8_t BOARD_WHEEL_ANIM_FRAMES = 8;
constexpr uint8_t MENU_STACK_CAPACITY = 7;
constexpr uint8_t MENU_BALL_COUNT = 6;

enum AppScene : uint8_t {
  SCENE_MAIN_MENU,
  SCENE_PLAYING,
  SCENE_DEMO,
};

enum TransitionMode : uint8_t {
  TRANSITION_NONE,
  TRANSITION_CURTAIN_TO_DEMO,
  TRANSITION_CURTAIN_TO_MENU,
};

enum ConfirmAction : uint8_t {
  CONFIRM_NONE,
  CONFIRM_MAIN_MENU,
};

enum OpponentMode : uint8_t {
  OPPONENT_PLAYER_TWO,
  OPPONENT_CPU_EASY,
  OPPONENT_CPU_HARD,
  OPPONENT_LINK,
  OPPONENT_MODE_COUNT,
};

enum RulesetMode : uint8_t {
  RULESET_STANDARD,
  RULESET_LONG,
  RULESET_LESKER,
  RULESET_MODE_COUNT,
};

enum MenuSideAnimation : uint8_t {
  MENU_ANIM_STACK,
  MENU_ANIM_BALLS,
};

enum AudioMode : uint8_t {
  AUDIO_MUSIC_FX,
  AUDIO_FX_ONLY,
  AUDIO_MUTED,
};

enum CpuAnimationPhase : uint8_t {
  CPU_ANIM_IDLE,
  CPU_ANIM_TO_FROM,
  CPU_ANIM_PICK,
  CPU_ANIM_TO_TO,
  CPU_ANIM_DROP,
};

#ifdef ALL_MENS_MORRIS_DEBUG
enum DebugScenario : uint8_t {
  DEBUG_SCENARIO_MILL,
  DEBUG_SCENARIO_FLY,
  DEBUG_SCENARIO_BLOCK,
  DEBUG_SCENARIO_DRAW,
  DEBUG_SCENARIO_COUNT,
};
#endif

MorrisGameState game;
MorrisGameState history[HISTORY_CAPACITY];
RuleSet activeRuleSet = ClassicRuleSet;
AppScene scene = SCENE_MAIN_MENU;
ConfirmAction confirmAction = CONFIRM_NONE;
uint8_t selectedMenuItem = 0;
uint8_t selectedBoardMenuItem = 0;
RulesetMode selectedRulesetMenuItem = RULESET_STANDARD;
Player firstPlayer = PLAYER_TWO;
Player cpuPlayer = PLAYER_ONE;
uint8_t historyCount = 0;
uint8_t historyIndex = 0;
OpponentMode opponentMode = OPPONENT_PLAYER_TWO;
bool linkWasAvailable = false;
uint8_t cpuThinkFrames = 0;
CpuAnimationPhase cpuAnimationPhase = CPU_ANIM_IDLE;
AiAction cpuAnimationAction = {NO_POINT, NO_POINT, TURN_ACTION_MOVE};
GamePhase cpuAnimationPhaseBeforeAction = PHASE_PLACING;
uint8_t cpuAnimationTarget = NO_POINT;
uint8_t cpuAnimationStepFrames = CPU_MIN_STEP_FRAMES;
uint8_t cpuAnimationStepTimer = 0;
uint8_t cpuAnimationPauseFrames = 0;
int8_t pendingCursorDx = 0;
int8_t pendingCursorDy = 0;
uint8_t pendingCursorFrames = 0;
uint8_t messageFrames = 0;
uint8_t millFlashFrames = 0;
uint8_t animationFrame = 0;
uint16_t idleFrames = 0;
uint16_t demoFrames = 0;
uint8_t demoGameOverFrames = 0;
TransitionMode transitionMode = TRANSITION_NONE;
uint8_t transitionFrame = 0;
uint8_t menuMusicRestartFrames = 0;
uint8_t menuMusicFramesRemaining = 0;
uint8_t menuMusicIndex = 0;
uint8_t menuMusicTheme = 0;
bool menuMusicActive = false;
AudioMode audioMode = AUDIO_MUSIC_FX;
uint8_t audioOsdFrames = 0;
uint8_t rejectFlashFrames = 0;
uint8_t previousBoardMenuSelection = 0;
uint8_t boardWheelFrames = 0;
int8_t boardWheelDirection = 0;
MenuSideAnimation menuSideAnimation = MENU_ANIM_STACK;
uint8_t menuStackCount[2] = {};
uint8_t menuStackPieces[2] = {};
uint8_t menuStackBlinkFrames = 0;
uint8_t menuFallingPiece = 0;
int8_t menuFallBottomY = 0;
uint8_t menuFallVelocity = 1;
uint8_t menuFallingColumn = 0;
#ifdef ALL_MENS_MORRIS_DEBUG
uint8_t debugScenario = DEBUG_SCENARIO_MILL;
#endif
const char *message = "";

void ledsOff();
void startMillEffects();
void clearPendingCursorDirection();
void showActionFeedback(GamePhase phaseBeforeAction, bool moved);
void playEffect(uint16_t freq, uint16_t dur);
void setCpuAnimationStepRate(const AiAction &action);
void enterCpuPickPause();
void enterCpuDropPause();
void playCpuCursorStepSound();
void playCpuActionSound();
uint8_t textPixelWidth(const char *text);
void resetMenuAnimation(bool advanceStyle);
void updateMenuAnimation();
void returnToMainMenu();

const BoardDefinition *boardProfile(uint8_t index) {
  if (index >= MORRIS_BOARD_PROFILE_COUNT) {
    return nullptr;
  }
  return reinterpret_cast<const BoardDefinition *>(pgm_read_ptr(&MorrisBoardProfiles[index]));
}

const RuleSet *ruleProfile(uint8_t index) {
  if (index >= MORRIS_BOARD_PROFILE_COUNT) {
    return nullptr;
  }
  return reinterpret_cast<const RuleSet *>(pgm_read_ptr(&MorrisRuleProfiles[index]));
}

bool selectedBoardIsPlayable() {
  return boardProfile(selectedBoardMenuItem) != nullptr && ruleProfile(selectedBoardMenuItem) != nullptr;
}

bool isCpuTurn() {
  return ((scene == SCENE_PLAYING
           && (opponentMode == OPPONENT_CPU_EASY || opponentMode == OPPONENT_CPU_HARD))
          || scene == SCENE_DEMO)
      && confirmAction == CONFIRM_NONE
      && game.phase != PHASE_GAME_OVER
      && (scene == SCENE_DEMO || game.currentPlayer == cpuPlayer);
}

bool isCpuAnimating() {
  return cpuAnimationPhase != CPU_ANIM_IDLE;
}

bool isRemoteLinkAnimating() {
  return scene == SCENE_PLAYING
      && opponentMode == OPPONENT_LINK
      && game.phase != PHASE_GAME_OVER
      && game.currentPlayer != linkLocalPlayer(firstPlayer)
      && isCpuAnimating();
}

bool linkModeAvailable() {
  return linkPeerAvailable();
}

bool linkDiscoveryMenuActive() {
  return scene == SCENE_MAIN_MENU && transitionMode == TRANSITION_NONE;
}

bool isLinkMatch() {
  return scene == SCENE_PLAYING && opponentMode == OPPONENT_LINK;
}

bool isLocalLinkTurn() {
  return !isLinkMatch()
      || game.phase == PHASE_GAME_OVER
      || game.currentPlayer == linkLocalPlayer(firstPlayer);
}

bool linkInputLocked() {
  return isLinkMatch() && !isLocalLinkTurn();
}

void setMessage(const char *text) {
  message = text;
  messageFrames = 45;
}

void rejectInput() {
  rejectFlashFrames = 6;
  setMessage("WAIT");
  playEffect(110, 35);
}

void markSoundEffect() {
  menuMusicActive = false;
  menuMusicRestartFrames = MENU_MUSIC_RESTART_FRAMES;
}

void playEffect(uint16_t freq, uint16_t dur) {
  if (audioMode == AUDIO_MUTED) {
    return;
  }
  markSoundEffect();
  sound.tone(freq, dur);
}

void playEffect(uint16_t freq1, uint16_t dur1, uint16_t freq2, uint16_t dur2) {
  if (audioMode == AUDIO_MUTED) {
    return;
  }
  markSoundEffect();
  sound.tone(freq1, dur1, freq2, dur2);
}

void playEffect(uint16_t freq1, uint16_t dur1, uint16_t freq2, uint16_t dur2,
                uint16_t freq3, uint16_t dur3) {
  if (audioMode == AUDIO_MUTED) {
    return;
  }
  markSoundEffect();
  sound.tone(freq1, dur1, freq2, dur2, freq3, dur3);
}

void stopMenuMusic() {
  if (menuMusicActive) {
    sound.noTone();
    menuMusicActive = false;
    menuMusicFramesRemaining = 0;
  }
}

void restartMenuMusic() {
  stopMenuMusic();
  menuMusicIndex = 0;
  menuMusicRestartFrames = MENU_MUSIC_INITIAL_DELAY_FRAMES;
}

void nextMenuMusicTheme() {
  menuMusicTheme++;
  if (menuMusicTheme >= MENU_MUSIC_THEME_COUNT) {
    menuMusicTheme = 0;
  }
}

uint16_t menuMusicDurationMs(uint8_t duration) {
  return static_cast<uint16_t>(duration) * MENU_MUSIC_TICK_MS;
}

uint8_t menuMusicDurationFrames(uint8_t duration) {
  uint16_t frames = (static_cast<uint16_t>(duration) * MENU_MUSIC_TICK_MS * GAME_FPS + 999) / 1000;
  return frames == 0 ? 1 : frames;
}

uint16_t menuMusicFrequency(uint8_t note) {
  if (note < MENU_MUSIC_MIN_NOTE || note > MENU_MUSIC_MAX_NOTE) {
    return 0;
  }
  return pgm_read_word(&MenuMusicFrequencies[note - MENU_MUSIC_MIN_NOTE]);
}

void startMenuMusicEvent() {
  uint8_t eventCount = pgm_read_byte(&MenuMusicThemeEventCounts[menuMusicTheme]);
  if (menuMusicIndex >= eventCount) {
    menuMusicIndex = 0;
  }

  const uint8_t *notes = reinterpret_cast<const uint8_t *>(pgm_read_ptr(&MenuMusicNotesByTheme[menuMusicTheme]));
  const uint8_t *durations = reinterpret_cast<const uint8_t *>(pgm_read_ptr(&MenuMusicDurationsByTheme[menuMusicTheme]));
  uint8_t note = pgm_read_byte(&notes[menuMusicIndex]);
  uint8_t duration = pgm_read_byte(&durations[menuMusicIndex]);
  menuMusicIndex++;
  menuMusicFramesRemaining = menuMusicDurationFrames(duration);
  menuMusicActive = true;

  uint16_t frequency = menuMusicFrequency(note);
  if (frequency == 0) {
    sound.noTone();
  } else {
    sound.tone(frequency, (menuMusicDurationMs(duration) * 3) / 4);
  }
}

void updateMenuMusic() {
  if (scene != SCENE_MAIN_MENU || audioMode != AUDIO_MUSIC_FX) {
    stopMenuMusic();
    return;
  }

  if (menuMusicActive && menuMusicFramesRemaining > 0) {
    menuMusicFramesRemaining--;
    return;
  }

  if (menuMusicRestartFrames > 0) {
    menuMusicRestartFrames--;
    return;
  }

  startMenuMusicEvent();
}

const char *audioModeLabel() {
  if (audioMode == AUDIO_FX_ONLY) {
    return "FX ONLY";
  }
  if (audioMode == AUDIO_MUTED) {
    return "MUTED";
  }
  return "MUSIC+FX";
}

void nextAudioMode() {
  if (audioMode == AUDIO_MUSIC_FX) {
    audioMode = AUDIO_FX_ONLY;
  } else if (audioMode == AUDIO_FX_ONLY) {
    audioMode = AUDIO_MUTED;
  } else {
    audioMode = AUDIO_MUSIC_FX;
  }
  stopMenuMusic();
  if (audioMode == AUDIO_MUSIC_FX) {
    restartMenuMusic();
  }
  audioOsdFrames = AUDIO_OSD_FRAMES;
}

const char *opponentModeLabel() {
  if (opponentMode == OPPONENT_LINK) {
    return "LINK";
  }
  if (opponentMode == OPPONENT_CPU_EASY) {
    return "CPU EASY";
  }
  if (opponentMode == OPPONENT_CPU_HARD) {
    return "CPU HARD";
  }
  return "PLAYER 2";
}

const char *rulesetModeLabel() {
  if (selectedRulesetMenuItem == RULESET_LONG) {
    return "LONG";
  }
  if (selectedRulesetMenuItem == RULESET_LESKER) {
    return "LESKER";
  }
  return "STANDARD";
}

uint8_t scaledPieceCount(uint8_t basePieces) {
  if (selectedRulesetMenuItem == RULESET_LONG) {
    return static_cast<uint8_t>(basePieces * 2 + 3);
  }
  if (selectedRulesetMenuItem == RULESET_LESKER) {
    return static_cast<uint8_t>(basePieces + 1);
  }
  return basePieces;
}

const RuleSet *buildActiveRuleSet(const RuleSet *standard) {
  if (standard == nullptr) {
    standard = &ClassicRuleSet;
  }

  activeRuleSet = *standard;
  activeRuleSet.piecesPerPlayer = scaledPieceCount(standard->piecesPerPlayer);
  if (selectedRulesetMenuItem == RULESET_LONG) {
    activeRuleSet.noCaptureDrawTurnLimit = 100;
    activeRuleSet.placementStopEmptyPoints = 1;
    activeRuleSet.flags |= RULE_MIXED_PLACEMENT_MOVEMENT | RULE_SKIP_BLOCKED_WITH_RESERVE;
    activeRuleSet.flags &= ~RULE_MATERIAL_WIN_REQUIRES_RESERVE_EMPTY;
  } else if (selectedRulesetMenuItem == RULESET_LESKER) {
    activeRuleSet.flags |= RULE_MIXED_PLACEMENT_MOVEMENT | RULE_START_IN_MOVING;
    activeRuleSet.flags &= ~RULE_SKIP_BLOCKED_WITH_RESERVE;
  }
  return &activeRuleSet;
}

AiDifficulty selectedAiDifficulty() {
  if (scene == SCENE_DEMO) {
    return game.currentPlayer == PLAYER_ONE ? AI_EASY : AI_HARD;
  }
  return opponentMode == OPPONENT_CPU_HARD ? AI_HARD : AI_EASY;
}

uint8_t cpuThinkDelayFrames() {
  if (scene == SCENE_DEMO) {
    return random(DEMO_CPU_MIN_THINK_FRAMES, DEMO_CPU_MAX_THINK_FRAMES + 1);
  }
  return CPU_THINK_FRAMES;
}

void nextOpponentMode() {
  do {
    opponentMode = static_cast<OpponentMode>((opponentMode + 1) % OPPONENT_MODE_COUNT);
  } while (opponentMode == OPPONENT_LINK && !linkModeAvailable());
}

void previousOpponentMode() {
  do {
    opponentMode = opponentMode == OPPONENT_PLAYER_TWO
        ? static_cast<OpponentMode>(OPPONENT_MODE_COUNT - 1)
        : static_cast<OpponentMode>(opponentMode - 1);
  } while (opponentMode == OPPONENT_LINK && !linkModeAvailable());
}

void startBoardWheelAnimation(uint8_t oldBoard, int8_t direction) {
  previousBoardMenuSelection = oldBoard;
  boardWheelDirection = direction;
  boardWheelFrames = BOARD_WHEEL_ANIM_FRAMES;
}

void nextBoardMenuItem() {
  uint8_t oldBoard = selectedBoardMenuItem;
  selectedBoardMenuItem = (selectedBoardMenuItem + 1) % BOARD_MENU_COUNT;
  startBoardWheelAnimation(oldBoard, 1);
}

void previousBoardMenuItem() {
  uint8_t oldBoard = selectedBoardMenuItem;
  selectedBoardMenuItem = selectedBoardMenuItem == 0 ? BOARD_MENU_COUNT - 1 : selectedBoardMenuItem - 1;
  startBoardWheelAnimation(oldBoard, -1);
}

void nextRulesetMode() {
  selectedRulesetMenuItem = static_cast<RulesetMode>((selectedRulesetMenuItem + 1) % RULESET_MODE_COUNT);
}

void previousRulesetMode() {
  selectedRulesetMenuItem = selectedRulesetMenuItem == RULESET_STANDARD
      ? static_cast<RulesetMode>(RULESET_MODE_COUNT - 1)
      : static_cast<RulesetMode>(selectedRulesetMenuItem - 1);
}

BoardPoint screenPoint(uint8_t index) {
  BoardPoint point = boardPoint(*game.board, index);
  point.x += BOARD_OFFSET_X;
  point.y += BOARD_OFFSET_Y;
  return point;
}

bool gameStateChanged(const MorrisGameState &before, const MorrisGameState &after) {
  if (before.board != after.board
      || before.rules != after.rules
      || before.currentPlayer != after.currentPlayer
      || before.winner != after.winner
      || before.phase != after.phase
      || before.winReason != after.winReason
      || before.phaseAfterCapture != after.phaseAfterCapture
      || before.actionMode != after.actionMode
      || before.stateFlags != after.stateFlags
      || before.turnsSinceCapture != after.turnsSinceCapture) {
    return true;
  }

  for (uint8_t i = 0; i < 2; i++) {
    if (before.piecesToPlace[i] != after.piecesToPlace[i]
        || before.piecesOnBoard[i] != after.piecesOnBoard[i]) {
      return true;
    }
  }

  for (uint8_t point = 0; point < before.board->pointCount; point++) {
    if (pointAt(before, point) != pointAt(after, point)) {
      return true;
    }
  }
  return false;
}

void resetHistory() {
  history[0] = game;
  historyCount = 1;
  historyIndex = 0;
}

void pushHistory() {
  if (historyIndex + 1 < historyCount) {
    historyCount = historyIndex + 1;
  }
  if (historyCount < HISTORY_CAPACITY) {
    history[historyCount++] = game;
    historyIndex = historyCount - 1;
    return;
  }
  for (uint8_t i = 1; i < HISTORY_CAPACITY; i++) {
    history[i - 1] = history[i];
  }
  history[HISTORY_CAPACITY - 1] = game;
  historyIndex = HISTORY_CAPACITY - 1;
  historyCount = HISTORY_CAPACITY;
}

bool undoHistory() {
  if (historyIndex == 0 || historyCount == 0) {
    return false;
  }
  historyIndex--;
  game = history[historyIndex];
  return true;
}

bool redoHistory() {
  if (historyIndex + 1 >= historyCount) {
    return false;
  }
  historyIndex++;
  game = history[historyIndex];
  return true;
}

void startMatch() {
  const BoardDefinition *board = boardProfile(selectedBoardMenuItem);
  const RuleSet *standardRules = ruleProfile(selectedBoardMenuItem);
  if (board == nullptr || standardRules == nullptr) {
    board = &ClassicBoardDefinition;
    standardRules = &ClassicRuleSet;
  }
  const RuleSet *rules = buildActiveRuleSet(standardRules);
  resetMorrisGame(game, *board, *rules);
  game.currentPlayer = firstPlayer;
  cpuPlayer = opponentOf(firstPlayer);
  resetHistory();
  clearPendingCursorDirection();
  millFlashFrames = 0;
  idleFrames = 0;
  demoFrames = 0;
  demoGameOverFrames = 0;
  confirmAction = CONFIRM_NONE;
  scene = SCENE_PLAYING;
  cpuThinkFrames = isCpuTurn() ? cpuThinkDelayFrames() : 0;
  setMessage("");
}

void startLinkMatch(uint8_t boardIndex, uint8_t rulesetIndex, Player startingPlayer) {
  selectedBoardMenuItem = boardIndex < BOARD_MENU_COUNT ? boardIndex : 0;
  selectedRulesetMenuItem = rulesetIndex < RULESET_MODE_COUNT
      ? static_cast<RulesetMode>(rulesetIndex)
      : RULESET_STANDARD;
  firstPlayer = startingPlayer == PLAYER_ONE ? PLAYER_ONE : PLAYER_TWO;
  opponentMode = OPPONENT_LINK;
  startMatch();
}

void applyLinkedCursor(const LinkEvent &event) {
  if (!isLinkMatch() || event.kind != LINK_EVENT_CURSOR || isLocalLinkTurn()) {
    return;
  }
  if (event.to >= game.board->pointCount) {
    return;
  }
  if (game.cursor != event.to) {
    game.cursor = event.to;
    playCpuCursorStepSound();
  }
}

void applyLinkedAction(const LinkEvent &event) {
  if (!isLinkMatch() || event.kind != LINK_EVENT_ACTION || isLocalLinkTurn()) {
    return;
  }
  if (event.to == NO_POINT) {
    bool redo = event.mode == TURN_ACTION_MOVE;
    if (redo ? redoHistory() : undoHistory()) {
      setMessage(redo ? "REDO" : "REWIND");
    }
    return;
  }
  cpuAnimationPhaseBeforeAction = game.phase;
  game.actionMode = event.mode;
  if (event.to < game.board->pointCount) {
    game.cursor = event.to;
  }
  if (event.from < game.board->pointCount) {
    game.selected = event.from;
  }
  playCpuActionSound();
  bool moved = applyPrimaryAction(game);
  if (moved) {
    pushHistory();
    showActionFeedback(cpuAnimationPhaseBeforeAction, true);
  } else {
    setMessage("LINK ERR");
  }
}

void handleLinkEvents() {
  bool linkAvailable = linkModeAvailable();
  bool discoveryActive = linkDiscoveryMenuActive();
  if (discoveryActive && linkAvailable && opponentMode != OPPONENT_LINK) {
    opponentMode = OPPONENT_LINK;
    if (!linkWasAvailable) {
      setMessage("LINK");
      playEffect(880, 35, 1175, 45);
    }
  }
  linkWasAvailable = linkAvailable;

  LinkEvent event;
  while (linkConsumeEvent(event)) {
    if (event.kind == LINK_EVENT_START && linkDiscoveryMenuActive()) {
      startLinkMatch(event.board, event.ruleset, event.firstPlayer);
    } else if (event.kind == LINK_EVENT_ACTION) {
      applyLinkedAction(event);
    } else if (event.kind == LINK_EVENT_CURSOR) {
      applyLinkedCursor(event);
    } else if (event.kind == LINK_EVENT_MENU && isLinkMatch()) {
      returnToMainMenu();
    }
  }
  if (discoveryActive && !linkAvailable && opponentMode == OPPONENT_LINK) {
    opponentMode = OPPONENT_PLAYER_TWO;
  }
}

uint8_t randomPlayableBoardIndex() {
  uint8_t start = random(BOARD_MENU_COUNT);
  for (uint8_t offset = 0; offset < BOARD_MENU_COUNT; offset++) {
    uint8_t index = (start + offset) % BOARD_MENU_COUNT;
    if (boardProfile(index) != nullptr
        && boardProfile(index) != &HexTwelveBoardDefinition
        && ruleProfile(index) != nullptr) {
      return index;
    }
  }
  return 0;
}

void applyDemoPreflightActions(uint8_t actionCount) {
  for (uint8_t i = 0; i < actionCount && game.phase != PHASE_GAME_OVER; i++) {
    AiAction action;
    MorrisGameState result;
    AiDifficulty difficulty = game.currentPlayer == PLAYER_ONE ? AI_EASY : AI_HARD;
    if (!chooseAiAction(game, difficulty, action, result)) {
      break;
    }
    game = result;
  }
}

void startDemoMatch() {
  uint8_t boardIndex = randomPlayableBoardIndex();
  const BoardDefinition *board = boardProfile(boardIndex);
  const RuleSet *rules = ruleProfile(boardIndex);
  if (board == nullptr || rules == nullptr) {
    board = &ClassicBoardDefinition;
    rules = &ClassicRuleSet;
  }

  resetMorrisGame(game, *board, *rules);
  game.currentPlayer = random(2) == 0 ? PLAYER_ONE : PLAYER_TWO;
  game.cursor = random(game.board->pointCount);
  game.selected = NO_POINT;
  applyDemoPreflightActions(DEMO_MIN_PREFLIGHT_ACTIONS + random(DEMO_PREFLIGHT_ACTION_RANGE));
  if (game.phase == PHASE_GAME_OVER) {
    resetMorrisGame(game, *board, *rules);
  }

  historyCount = 0;
  historyIndex = 0;
  clearPendingCursorDirection();
  millFlashFrames = 0;
  confirmAction = CONFIRM_NONE;
  scene = SCENE_DEMO;
  demoFrames = 0;
  demoGameOverFrames = 0;
  cpuThinkFrames = cpuThinkDelayFrames();
  setMessage("DEMO");
  stopMenuMusic();
}

void returnToMainMenu() {
  scene = SCENE_MAIN_MENU;
  confirmAction = CONFIRM_NONE;
  historyCount = 0;
  historyIndex = 0;
  cpuAnimationPhase = CPU_ANIM_IDLE;
  clearPendingCursorDirection();
  idleFrames = 0;
  demoFrames = 0;
  demoGameOverFrames = 0;
  millFlashFrames = 0;
  setMessage("");
  ledsOff();
  nextMenuMusicTheme();
  resetMenuAnimation(true);
  restartMenuMusic();
}

void startTransition(TransitionMode mode) {
  transitionMode = mode;
  transitionFrame = 0;
  if (mode == TRANSITION_CURTAIN_TO_DEMO) {
    stopMenuMusic();
  }
}

bool winnerIsCpu() {
  return scene != SCENE_DEMO
      && (opponentMode == OPPONENT_CPU_EASY || opponentMode == OPPONENT_CPU_HARD)
      && game.winner == cpuPlayer;
}

bool winnerIsPlayerOneSlot() {
  if (game.winner == PLAYER_NONE) {
    return false;
  }
  if (scene == SCENE_DEMO) {
    return game.winner == PLAYER_ONE;
  }
  if (opponentMode == OPPONENT_CPU_EASY || opponentMode == OPPONENT_CPU_HARD) {
    return !winnerIsCpu();
  }
  return game.winner == firstPlayer;
}

void playGameOverFanfare() {
  if (game.winner == PLAYER_NONE) {
    playEffect(392, 90, 330, 90, 262, 180);
  } else if (winnerIsCpu()) {
    playEffect(330, 90, 247, 100, 196, 180);
  } else {
    playEffect(523, 80, 659, 80, 1047, 180);
  }
}

void showActionFeedback(GamePhase phaseBeforeAction, bool moved) {
  if (!moved) {
    return;
  }

  if (game.phase == PHASE_GAME_OVER) {
    if (game.winner == PLAYER_NONE) {
      setMessage("DRAW");
    } else {
      setMessage(game.winner == PLAYER_ONE ? "B WIN" : "W WIN");
    }
    playGameOverFanfare();
  } else if (gameStateFlag(game, MORRIS_STATE_LAST_MOVE_MADE_MILL)) {
    setMessage("MILL!");
    startMillEffects();
  } else if (phaseBeforeAction == PHASE_CAPTURING) {
    setMessage("TAKEN");
    playEffect(330, 60, 220, 80);
  } else {
    setMessage("");
    playEffect(440, 30);
  }
}

#ifdef ALL_MENS_MORRIS_DEBUG
void clearDebugBoard() {
  for (uint8_t point = 0; point < MORRIS_MAX_POINT_COUNT; point++) {
    setPointAt(game, point, PLAYER_NONE);
  }
  game.cursor = 0;
  game.selected = NO_POINT;
  game.winner = PLAYER_NONE;
  game.winReason = WIN_NONE;
  game.phaseAfterCapture = PHASE_PLACING;
  game.stateFlags = 0;
  game.turnsSinceCapture = 0;
  game.piecesToPlace[0] = 0;
  game.piecesToPlace[1] = 0;
  game.piecesOnBoard[0] = 0;
  game.piecesOnBoard[1] = 0;
}

void putDebugPiece(uint8_t point, Player player) {
  setPointAt(game, point, player);
  if (player != PLAYER_NONE) {
    game.piecesOnBoard[playerIndex(player)]++;
  }
}

void loadDebugMillScenario() {
  resetMorrisGame(game);
  clearDebugBoard();
  game.phase = PHASE_PLACING;
  game.currentPlayer = PLAYER_TWO;
  game.piecesToPlace[0] = 7;
  game.piecesToPlace[1] = 7;
  putDebugPiece(0, PLAYER_TWO);
  putDebugPiece(1, PLAYER_TWO);
  putDebugPiece(9, PLAYER_ONE);
  putDebugPiece(10, PLAYER_ONE);
  game.cursor = 2;
  setMessage("DBG MILL");
}

void loadDebugFlyScenario() {
  resetMorrisGame(game);
  clearDebugBoard();
  game.phase = PHASE_MOVING;
  game.currentPlayer = PLAYER_TWO;
  putDebugPiece(0, PLAYER_TWO);
  putDebugPiece(9, PLAYER_TWO);
  putDebugPiece(21, PLAYER_TWO);
  putDebugPiece(2, PLAYER_ONE);
  putDebugPiece(14, PLAYER_ONE);
  putDebugPiece(23, PLAYER_ONE);
  putDebugPiece(4, PLAYER_ONE);
  game.cursor = 0;
  setMessage("DBG FLY");
}

void loadDebugBlockScenario() {
  resetMorrisGame(game);
  clearDebugBoard();
  game.phase = PHASE_GAME_OVER;
  game.currentPlayer = PLAYER_ONE;
  game.winner = PLAYER_TWO;
  game.winReason = WIN_BY_BLOCK;
  putDebugPiece(0, PLAYER_ONE);
  putDebugPiece(2, PLAYER_ONE);
  putDebugPiece(9, PLAYER_ONE);
  putDebugPiece(1, PLAYER_TWO);
  putDebugPiece(10, PLAYER_TWO);
  putDebugPiece(14, PLAYER_TWO);
  putDebugPiece(21, PLAYER_TWO);
  game.cursor = 0;
  setMessage("DBG BLOCK");
}

void loadDebugDrawScenario() {
  resetMorrisGame(game);
  clearDebugBoard();
  game.phase = PHASE_MOVING;
  game.currentPlayer = PLAYER_TWO;
  game.turnsSinceCapture = DRAW_NO_CAPTURE_TURN_LIMIT - 1;
  putDebugPiece(0, PLAYER_TWO);
  putDebugPiece(9, PLAYER_TWO);
  putDebugPiece(21, PLAYER_TWO);
  putDebugPiece(2, PLAYER_ONE);
  putDebugPiece(14, PLAYER_ONE);
  putDebugPiece(23, PLAYER_ONE);
  game.cursor = 0;
  setMessage("DBG DRAW");
}

void loadDebugScenario() {
  historyCount = 0;
  historyIndex = 0;
  millFlashFrames = 0;
  confirmAction = CONFIRM_NONE;
  scene = SCENE_PLAYING;
  arduboy.digitalWriteRGB(RGB_OFF, RGB_OFF, RGB_OFF);

  if (debugScenario == DEBUG_SCENARIO_MILL) {
    loadDebugMillScenario();
  } else if (debugScenario == DEBUG_SCENARIO_FLY) {
    loadDebugFlyScenario();
  } else if (debugScenario == DEBUG_SCENARIO_BLOCK) {
    loadDebugBlockScenario();
  } else {
    loadDebugDrawScenario();
  }

  debugScenario = (debugScenario + 1) % DEBUG_SCENARIO_COUNT;
  playEffect(988, 35, 740, 50);
}
#endif

uint16_t absoluteDelta(int16_t value) {
  return value < 0 ? -value : value;
}

uint8_t cursorToward(int8_t dx, int8_t dy) {
  BoardPoint current = boardPoint(*game.board, game.cursor);
  uint8_t bestNeighbor = NO_POINT;
  uint8_t bestAny = NO_POINT;
  uint16_t bestNeighborScore = 65535;
  uint16_t bestAnyScore = 65535;

  for (uint8_t point = 0; point < game.board->pointCount; point++) {
    if (point == game.cursor) {
      continue;
    }

    BoardPoint candidate = boardPoint(*game.board, point);
    int16_t deltaX = static_cast<int16_t>(candidate.x) - current.x;
    int16_t deltaY = static_cast<int16_t>(candidate.y) - current.y;
    int16_t dot = deltaX * dx + deltaY * dy;
    if (dot <= 0) {
      continue;
    }

    uint16_t score = absoluteDelta(deltaX * dy - deltaY * dx) * 32
        + absoluteDelta(deltaX) + absoluteDelta(deltaY);
    if (score < bestAnyScore) {
      bestAnyScore = score;
      bestAny = point;
    }

    for (uint8_t slot = 0; slot < adjacencyCount(*game.board, game.cursor); slot++) {
      if (adjacentPoint(*game.board, game.cursor, slot) == point && score < bestNeighborScore) {
        bestNeighborScore = score;
        bestNeighbor = point;
      }
    }
  }
  return bestNeighbor != NO_POINT ? bestNeighbor : bestAny;
}

void moveCursorToward(int8_t dx, int8_t dy) {
  uint8_t next = cursorToward(dx, dy);
  if (next != NO_POINT) {
    game.cursor = next;
    if (isLinkMatch() && isLocalLinkTurn()) {
      linkSendCursor(game.cursor);
    }
  }
}

void clearPendingCursorDirection() {
  pendingCursorDx = 0;
  pendingCursorDy = 0;
  pendingCursorFrames = 0;
}

bool currentDpadDirection(int8_t &dx, int8_t &dy) {
  dx = 0;
  dy = 0;
  if (arduboy.pressed(LEFT_BUTTON) && !arduboy.pressed(RIGHT_BUTTON)) {
    dx = -1;
  } else if (arduboy.pressed(RIGHT_BUTTON) && !arduboy.pressed(LEFT_BUTTON)) {
    dx = 1;
  }
  if (arduboy.pressed(UP_BUTTON) && !arduboy.pressed(DOWN_BUTTON)) {
    dy = -1;
  } else if (arduboy.pressed(DOWN_BUTTON) && !arduboy.pressed(UP_BUTTON)) {
    dy = 1;
  }
  return dx != 0 || dy != 0;
}

bool dpadJustPressed() {
  return arduboy.justPressed(LEFT_BUTTON)
      || arduboy.justPressed(RIGHT_BUTTON)
      || arduboy.justPressed(UP_BUTTON)
      || arduboy.justPressed(DOWN_BUTTON);
}

bool anyButtonJustPressed() {
  return arduboy.justPressed(LEFT_BUTTON)
      || arduboy.justPressed(RIGHT_BUTTON)
      || arduboy.justPressed(UP_BUTTON)
      || arduboy.justPressed(DOWN_BUTTON)
      || arduboy.justPressed(A_BUTTON)
      || arduboy.justPressed(B_BUTTON);
}

void handleCursorDirectionInput() {
  int8_t dx = 0;
  int8_t dy = 0;
  bool hasDirection = currentDpadDirection(dx, dy);

  if (pendingCursorFrames == 0 && !dpadJustPressed()) {
    return;
  }

  if (hasDirection) {
    pendingCursorDx = dx;
    pendingCursorDy = dy;
    if (dx != 0 && dy != 0) {
      moveCursorToward(dx, dy);
      clearPendingCursorDirection();
      return;
    }
  }

  if (pendingCursorFrames == 0) {
    pendingCursorFrames = CURSOR_CHORD_FRAMES;
    return;
  }

  pendingCursorFrames--;
  if (pendingCursorFrames > 0) {
    return;
  }

  if (pendingCursorDx != 0 || pendingCursorDy != 0) {
    moveCursorToward(pendingCursorDx, pendingCursorDy);
  }
  clearPendingCursorDirection();
}

uint8_t nextGraphPointToward(uint8_t start, uint8_t target) {
  if (start == target || start >= game.board->pointCount || target >= game.board->pointCount) {
    return target;
  }

  uint8_t queue[MORRIS_MAX_POINT_COUNT];
  uint8_t previous[MORRIS_MAX_POINT_COUNT];
  uint8_t head = 0;
  uint8_t tail = 0;
  for (uint8_t i = 0; i < MORRIS_MAX_POINT_COUNT; i++) {
    previous[i] = NO_POINT;
  }

  queue[tail++] = start;
  previous[start] = start;
  while (head < tail) {
    uint8_t point = queue[head++];
    if (point == target) {
      break;
    }
    for (uint8_t slot = 0; slot < adjacencyCount(*game.board, point); slot++) {
      uint8_t adjacent = adjacentPoint(*game.board, point, slot);
      if (adjacent != 255 && previous[adjacent] == NO_POINT) {
        previous[adjacent] = point;
        queue[tail++] = adjacent;
      }
    }
  }

  if (previous[target] == NO_POINT) {
    return target;
  }

  uint8_t step = target;
  while (previous[step] != start) {
    step = previous[step];
  }
  return step;
}

uint8_t graphDistance(uint8_t start, uint8_t target) {
  if (start == target) {
    return 0;
  }

  uint8_t queue[MORRIS_MAX_POINT_COUNT];
  uint8_t distance[MORRIS_MAX_POINT_COUNT];
  uint8_t head = 0;
  uint8_t tail = 0;
  for (uint8_t i = 0; i < MORRIS_MAX_POINT_COUNT; i++) {
    distance[i] = NO_POINT;
  }

  queue[tail++] = start;
  distance[start] = 0;
  while (head < tail) {
    uint8_t point = queue[head++];
    for (uint8_t slot = 0; slot < adjacencyCount(*game.board, point); slot++) {
      uint8_t adjacent = adjacentPoint(*game.board, point, slot);
      if (adjacent == 255 || distance[adjacent] != NO_POINT) {
        continue;
      }
      distance[adjacent] = distance[point] + 1;
      if (adjacent == target) {
        return distance[adjacent];
      }
      queue[tail++] = adjacent;
    }
  }
  return 1;
}

void ledsOff() {
  arduboy.digitalWriteRGB(RGB_OFF, RGB_OFF, RGB_OFF);
}

bool spritePixelIsBlack(const uint8_t *sprite, uint8_t frame, uint8_t x, uint8_t y) {
  uint8_t width = pgm_read_byte(sprite);
  uint8_t height = pgm_read_byte(sprite + 1);
  uint8_t pages = (height + 7) / 8;
  uint16_t frameOffset = static_cast<uint16_t>(frame) * width * pages;
  uint8_t byte = pgm_read_byte(sprite + 2 + frameOffset + (y / 8) * width + x);
  return ((byte >> (y % 8)) & 1) == 0;
}

void drawSpriteWhitePixelsAsBlack(const uint8_t *sprite, uint8_t frame, int16_t x, int16_t y) {
  uint8_t width = pgm_read_byte(sprite);
  uint8_t height = pgm_read_byte(sprite + 1);
  for (uint8_t py = 0; py < height; py++) {
    for (uint8_t px = 0; px < width; px++) {
      int16_t drawX = x + px;
      int16_t drawY = y + py;
      if (drawX >= 0 && drawX < 128 && drawY >= 0 && drawY < 64
          && !spritePixelIsBlack(sprite, frame, px, py)) {
        arduboy.drawPixel(drawX, drawY, BLACK);
      }
    }
  }
}

void drawBootLogo(uint8_t frame) {
  int8_t dx = (frame % 4 == 0) ? -2 : (frame % 4 == 2) ? 2 : 0;
  int8_t dy = (frame % 6 == 1) ? -1 : (frame % 6 == 4) ? 1 : 0;
  drawSpriteWhitePixelsAsBlack(bootLogo24x12, 0, BOOT_LOGO_X + dx, BOOT_LOGO_Y + dy);
}

void drawBootParticles(uint8_t frame) {
  for (uint8_t i = 0; i < 22; i++) {
    uint8_t fallSpeed = 1 + (i % 3);
    int16_t x = (i * 23 + frame / 2 + ((frame + i * 7) / 8) % 7) % 128;
    int16_t y = (i * 19 + frame * fallSpeed) % 72 - 8;
    if (y >= 0 && y < 64) {
      arduboy.drawPixel(x, y, BLACK);
    }
  }
}

uint16_t bootFillPixelCount(uint8_t frame) {
  uint16_t count = 0;
  for (uint8_t i = 0; i <= frame; i++) {
    if (i < framesAtGameFps(5)) {
      count += 10;
    } else if (i < framesAtGameFps(12)) {
      count += 34;
    } else {
      count += 420;
    }
  }
  return count;
}

uint16_t bootFillPixelPosition(uint16_t index) {
  uint16_t value = index * 173U + 911U;
  value ^= value >> 7;
  value *= 197U;
  value ^= value >> 5;
  return value & 8191;
}

void drawBootFillPixels(uint8_t frame) {
  uint16_t count = bootFillPixelCount(frame);
  if (count > 8192) {
    count = 8192;
  }

  for (uint16_t i = 0; i < count; i++) {
    uint16_t p = bootFillPixelPosition(i);
    arduboy.drawPixel(p & 127, p >> 7, BLACK);
  }
}

void drawBootCurtain(uint8_t frame) {
  uint8_t halfWidth = 3 + frame * 5 + (frame * frame) / 8;
  if (halfWidth > 64) {
    halfWidth = 64;
  }
  arduboy.fillRect(64 - halfWidth, 0, halfWidth * 2, 64, BLACK);
}

void playBootAnimation() {
  ledsOff();
  for (uint8_t frame = 0; frame < BOOT_TOTAL_FRAMES; frame++) {
    while (!arduboy.nextFrame()) { }
    arduboy.pollButtons();
    if (arduboy.buttonsState()) {
      break;
    }

    arduboy.fillScreen(WHITE);
    drawBootLogo(frame);
    if (frame >= BOOT_DUST_START_FRAMES) {
      drawBootParticles(frame - BOOT_DUST_START_FRAMES);
    }
    if (frame >= BOOT_FILL_START_FRAMES) {
      drawBootFillPixels(frame - BOOT_FILL_START_FRAMES);
    }
    if (frame >= BOOT_CURTAIN_START_FRAMES) {
      drawBootCurtain(frame - BOOT_CURTAIN_START_FRAMES);
    }
    arduboy.display();
  }
  arduboy.fillScreen(BLACK);
  arduboy.display();
  ledsOff();
}

void startMillEffects() {
  millFlashFrames = 36;
  playEffect(659, 70, 880, 90, 1175, 120);
}

void updateMillLed() {
  if (millFlashFrames == 0) {
    if (isLinkMatch() && isLocalLinkTurn()) {
      arduboy.digitalWriteRGB(RGB_OFF, RGB_ON, RGB_OFF);
    } else {
      ledsOff();
    }
    return;
  }

  millFlashFrames--;
  if ((millFlashFrames / 4) % 2 == 0) {
    arduboy.digitalWriteRGB(RGB_ON, RGB_OFF, RGB_OFF);
  } else {
    arduboy.digitalWriteRGB(RGB_OFF, RGB_OFF, RGB_ON);
  }
}

void drawBoardLine(uint8_t a, uint8_t b) {
  BoardPoint pa = screenPoint(a);
  BoardPoint pb = screenPoint(b);
  arduboy.drawLine(pa.x, pa.y, pb.x, pb.y, BLACK);
}

void drawBoard() {
  for (uint8_t point = 0; point < game.board->pointCount; point++) {
    for (uint8_t slot = 0; slot < adjacencyCount(*game.board, point); slot++) {
      uint8_t adjacent = adjacentPoint(*game.board, point, slot);
      if (adjacent != 255 && adjacent > point) {
        drawBoardLine(point, adjacent);
      }
    }
  }
}

void drawPiece(uint8_t point, Player player) {
  BoardPoint p = screenPoint(point);
  if (player == PLAYER_ONE) {
    arduboy.fillCircle(p.x, p.y, 3, BLACK);
  } else if (player == PLAYER_TWO) {
    arduboy.fillCircle(p.x, p.y, 3, WHITE);
    arduboy.drawCircle(p.x, p.y, 3, BLACK);
  } else {
    arduboy.fillRect(p.x - 1, p.y - 1, 3, 3, BLACK);
  }
}

bool dashPixelOn(uint8_t step, uint8_t phase) {
  return ((step + phase) % 8) < 4;
}

void drawDashedHLine(int8_t x, int8_t y, uint8_t w, uint8_t phase, uint8_t color) {
  for (uint8_t i = 0; i < w; i++) {
    if (dashPixelOn(i, phase)) {
      arduboy.drawPixel(x + i, y, color);
    }
  }
}

void drawDashedVLine(int8_t x, int8_t y, uint8_t h, uint8_t phase, uint8_t color) {
  for (uint8_t i = 0; i < h; i++) {
    if (dashPixelOn(i, phase)) {
      arduboy.drawPixel(x, y + i, color);
    }
  }
}

void drawDashedRect(int8_t x, int8_t y, uint8_t w, uint8_t h, uint8_t phase, uint8_t color) {
  drawDashedHLine(x, y, w, phase, color);
  drawDashedVLine(x + w - 1, y, h, phase + w, color);
  drawDashedHLine(x, y + h - 1, w, phase + w + h, color);
  drawDashedVLine(x, y, h, phase + h, color);
}

void drawCursor() {
  uint8_t phase = animationFrame / 4;
  BoardPoint p = screenPoint(game.cursor);
  drawDashedRect(p.x - 5, p.y - 5, 11, 11, phase, BLACK);
  if (game.selected != 255) {
    BoardPoint selected = screenPoint(game.selected);
    drawDashedRect(selected.x - 6, selected.y - 6, 13, 13, phase + 4, BLACK);
  }
}

void drawHudRule(uint8_t x) {
  for (uint8_t y = 2; y < 62; y += 4) {
    arduboy.drawPixel(x, y, WHITE);
    arduboy.drawPixel(x, y + 1, WHITE);
  }
}

void drawHudPiece(uint8_t x, uint8_t y, Player player) {
  if (player == PLAYER_ONE) {
    arduboy.drawCircle(x, y, 2, WHITE);
  } else {
    arduboy.fillCircle(x, y, 2, WHITE);
  }
}

const char *hudStatusLabel() {
  if (game.phase == PHASE_PLACING) {
    return "PUT";
  } else if (game.phase == PHASE_CAPTURING) {
    return "TAKE";
  } else if (game.phase == PHASE_GAME_OVER) {
    return "OVER";
  } else if (game.phase == PHASE_MOVING && game.selected == 255 && canPlaceAt(game, game.cursor)) {
    return "PUT";
  } else if (playerCanFly(game, game.currentPlayer)) {
    return "FLY";
  }
  return "MOVE";
}

void drawHudTextCentered(uint8_t panelX, uint8_t panelW, uint8_t y, const char *text) {
  tinyfont.setCursor(panelX + panelW / 2 - textPixelWidth(text) / 2, y);
  tinyfont.print(text);
}

void drawUiPieceSprite(const uint16_t *sprite, uint8_t x, uint8_t bottomY) {
  uint8_t topY = bottomY - HUD_PIECE_HEIGHT + 1;
  for (uint8_t y = 0; y < HUD_PIECE_HEIGHT; y++) {
    uint16_t mask = pgm_read_word(uiPieceMask16x10 + y);
    uint16_t row = pgm_read_word(sprite + y);
    for (uint8_t px = 0; px < HUD_PIECE_WIDTH; px++) {
      uint16_t bit = 1 << (HUD_PIECE_WIDTH - 1 - px);
      if (mask & bit) {
        arduboy.drawPixel(x + px, topY + y, row & bit ? WHITE : BLACK);
      }
    }
  }
}

void drawHudPieceStack(uint8_t x, Player player) {
  uint8_t count = game.piecesToPlace[playerIndex(player)];
  const uint16_t *sprite = player == PLAYER_ONE ? whiteUiPiece16x10 : blackUiPiece16x10;
  if (count == 0) {
    return;
  }
  if (count > 9) {
    drawUiPieceSprite(sprite, x, HUD_STACK_SINGLE_BOTTOM_Y);
    tinyfont.setCursor(x + 4, HUD_STACK_SINGLE_BOTTOM_Y + 4);
    tinyfont.print(count);
    return;
  }
  for (uint8_t i = 0; i < count; i++) {
    drawUiPieceSprite(sprite, x, HUD_STACK_BOTTOM_Y - i * HUD_STACK_STEP_Y);
  }
}

void drawHudPlayerPanel(uint8_t panelX, uint8_t panelW, Player player, const char *label) {
  uint8_t spriteX = panelX + panelW / 2 - HUD_PIECE_WIDTH / 2;
  drawHudTextCentered(panelX, panelW, 3, label);
  if (game.phase != PHASE_GAME_OVER && game.currentPlayer == player) {
    drawHudTextCentered(panelX, panelW, 12, hudStatusLabel());
  }
  drawHudPieceStack(spriteX, player);
}

void drawHud() {
  arduboy.fillRect(0, 0, 30, 64, BLACK);
  arduboy.fillRect(98, 0, 30, 64, BLACK);
  tinyfont.setTextColor(WHITE);

  drawHudRule(29);
  drawHudRule(98);

  Player leftPlayer = firstPlayer;
  Player rightPlayer = opponentOf(firstPlayer);
  drawHudPlayerPanel(0, 29, leftPlayer, "P1");
  drawHudPlayerPanel(99, 29, rightPlayer, "P2");
}

void drawCenteredPanel(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
  arduboy.fillRect(x, y, w, h, WHITE);
  arduboy.drawRect(x, y, w, h, BLACK);
  tinyfont.setTextColor(BLACK);
}

uint8_t textPixelWidth(const char *text) {
  uint8_t width = 0;
  while (*text != '\0') {
    width += 4;
    text++;
  }
  return width;
}

const char *boardMenuTitle(uint8_t boardIndex) {
  const BoardDefinition *board = boardProfile(boardIndex);
  if (board != nullptr) {
    return board->label;
  }
  return "SOON";
}

void startMenuFallingPiece() {
  uint8_t next = random(2);
  uint8_t count = menuStackCount[menuFallingColumn];
  uint8_t pieces = menuStackPieces[menuFallingColumn];
  if (count >= 2) {
    if (random(4) == 0) {
      next = (pieces >> (count - 1)) & 1;
    }
  }
  menuFallingPiece = next;
  menuFallBottomY = -1;
  menuFallVelocity = 1;
}

void resetMenuAnimation(bool advanceStyle) {
  if (advanceStyle) {
    menuSideAnimation = menuSideAnimation == MENU_ANIM_STACK ? MENU_ANIM_BALLS : MENU_ANIM_STACK;
  }
  menuStackCount[0] = 0;
  menuStackCount[1] = 0;
  menuStackPieces[0] = 0;
  menuStackPieces[1] = 0;
  menuStackBlinkFrames = 0;
  menuFallingColumn = 0;
  startMenuFallingPiece();
}

void updateMenuAnimation() {
  if (boardWheelFrames > 0) {
    boardWheelFrames--;
  }
  if (menuSideAnimation != MENU_ANIM_STACK) {
    return;
  }

  if (menuStackBlinkFrames > 0) {
    menuStackBlinkFrames--;
    uint8_t count = menuStackCount[menuFallingColumn];
    if (menuStackBlinkFrames == 0 && count >= 3) {
      count -= 3;
      menuStackCount[menuFallingColumn] = count;
      menuStackPieces[menuFallingColumn] &= (1 << count) - 1;
      menuFallingColumn ^= 1;
      startMenuFallingPiece();
    }
    return;
  }

  uint8_t count = menuStackCount[menuFallingColumn];
  int8_t targetY = 62 - count * HUD_STACK_STEP_Y;
  menuFallBottomY += menuFallVelocity;
  menuFallVelocity++;
  if (menuFallBottomY < targetY) {
    return;
  }

  menuFallBottomY = targetY;
  if (count >= MENU_STACK_CAPACITY) {
    count = 0;
    menuStackPieces[menuFallingColumn] = 0;
  }
  if (menuFallingPiece != 0) {
    menuStackPieces[menuFallingColumn] |= 1 << count;
  }
  count++;
  menuStackCount[menuFallingColumn] = count;
  if (count >= 3) {
    uint8_t pieces = menuStackPieces[menuFallingColumn];
    uint8_t top = (pieces >> (count - 1)) & 1;
    uint8_t second = (pieces >> (count - 2)) & 1;
    uint8_t third = (pieces >> (count - 3)) & 1;
    if (top == second && second == third) {
      menuStackBlinkFrames = 24;
      return;
    }
  }
  menuFallingColumn ^= 1;
  startMenuFallingPiece();
}

void drawMenuTitleShine() {
  uint8_t phase = (animationFrame >> 2) & 127;
  if (phase < 96) {
    return;
  }
  int8_t x = static_cast<int8_t>(30 + (phase - 96) * 4);
  arduboy.drawLine(x + 8, 4, x, 18, BLACK);
}

void drawBoardWheelText(const char *text, int8_t centerX) {
  tinyfont.setTextColor(BLACK);
  tinyfont.setCursor(centerX - textPixelWidth(text) / 2 - 6, BOARD_WHEEL_Y + 3);
  tinyfont.print(text);
}

void drawBoardWheel() {
  uint8_t midY = BOARD_WHEEL_Y + BOARD_WHEEL_H / 2;
  arduboy.fillRect(BOARD_WHEEL_X + 1, BOARD_WHEEL_Y, BOARD_WHEEL_W - 2, BOARD_WHEEL_H, WHITE);
  arduboy.fillRect(BOARD_WHEEL_X, BOARD_WHEEL_Y + 1, BOARD_WHEEL_W, BOARD_WHEEL_H - 2, WHITE);
  arduboy.fillRect(BOARD_WHEEL_X - 3, midY - 1, 3, 3, WHITE);
  arduboy.drawPixel(BOARD_WHEEL_X - 4, midY, WHITE);
  arduboy.fillRect(BOARD_WHEEL_X + BOARD_WHEEL_W, midY - 1, 3, 3, WHITE);
  arduboy.drawPixel(BOARD_WHEEL_X + BOARD_WHEEL_W + 3, midY, WHITE);

  int8_t centerX = BOARD_WHEEL_X + BOARD_WHEEL_W / 2;
  if (boardWheelFrames > 0 && boardWheelDirection != 0) {
    uint8_t progress = BOARD_WHEEL_ANIM_FRAMES - boardWheelFrames;
    int8_t offset = (static_cast<uint16_t>(progress) * BOARD_WHEEL_W) >> 3;
    int8_t oldCenter = centerX - boardWheelDirection * offset;
    int8_t newCenter = centerX + boardWheelDirection * (BOARD_WHEEL_W - offset);
    drawBoardWheelText(boardMenuTitle(previousBoardMenuSelection), oldCenter);
    drawBoardWheelText(boardMenuTitle(selectedBoardMenuItem), newCenter);
  } else {
    drawBoardWheelText(boardMenuTitle(selectedBoardMenuItem), centerX);
  }

  if (selectedMenuItem == 0) {
    drawDashedRect(BOARD_WHEEL_X - 5, BOARD_WHEEL_Y - 1, BOARD_WHEEL_W + 10, BOARD_WHEEL_H + 2,
        animationFrame / 5, WHITE);
  }
}

void drawMenuStackAnimation() {
  for (uint8_t column = 0; column < 2; column++) {
    uint8_t x = column == 0 ? 6 : 106;
    uint8_t count = menuStackCount[column];
    uint8_t pieces = menuStackPieces[column];
    for (uint8_t i = 0; i < count; i++) {
      if (column == menuFallingColumn && menuStackBlinkFrames > 0
          && i >= count - 3 && ((menuStackBlinkFrames >> 2) & 1)) {
        continue;
      }
      const uint16_t *sprite = ((pieces >> i) & 1) ? whiteUiPiece16x10 : blackUiPiece16x10;
      drawUiPieceSprite(sprite, x, 62 - i * HUD_STACK_STEP_Y);
    }
  }
  if (menuStackBlinkFrames == 0 && menuFallBottomY >= HUD_PIECE_HEIGHT - 1) {
    uint8_t x = menuFallingColumn == 0 ? 6 : 106;
    const uint16_t *sprite = menuFallingPiece ? whiteUiPiece16x10 : blackUiPiece16x10;
    drawUiPieceSprite(sprite, x, menuFallBottomY);
  }
}

void drawMenuBallAnimation() {
  for (uint8_t side = 0; side < 2; side++) {
    for (uint8_t i = 0; i < MENU_BALL_COUNT; i++) {
      uint8_t t = (animationFrame + i * 17 + side * 29) & 63;
      uint8_t x = (side == 0 ? 5 : 106) + ((i * 5 + (animationFrame >> 3)) & 15);
      uint8_t y = 4 + t;
      if (((i + side + (animationFrame >> 4)) & 1) == 0) {
        arduboy.fillCircle(x, y, 2, WHITE);
      } else {
        arduboy.drawCircle(x, y, 2, WHITE);
      }
    }
  }
}

void drawMenuSideAnimation() {
  if (menuSideAnimation == MENU_ANIM_STACK) {
    drawMenuStackAnimation();
  } else {
    drawMenuBallAnimation();
  }
}

void drawMainMenu() {
  arduboy.fillScreen(BLACK);
  tinyfont.setTextColor(WHITE);

  drawMenuSideAnimation();

  tinyfont.setCursor(43, 5);
  tinyfont.print("ALL MEN'S");
  tinyfont.setCursor(50, 12);
  tinyfont.print("MORRIS");
  drawMenuTitleShine();
  arduboy.drawLine(30, 20, 98, 20, WHITE);

  drawBoardWheel();
  if (!selectedBoardIsPlayable()) {
    tinyfont.setTextColor(WHITE);
    tinyfont.setCursor(108, 56);
    tinyfont.print("SOON");
  }

  tinyfont.setTextColor(WHITE);
  tinyfont.setCursor(38, 39);
  tinyfont.print(rulesetModeLabel());
  if (selectedMenuItem == 1) {
    drawDashedRect(35, 36, 62, 10, animationFrame / 5, WHITE);
  }

  tinyfont.setCursor(38, 49);
  tinyfont.print("FIRST ");
  tinyfont.print(firstPlayer == PLAYER_TWO ? "WHITE" : "BLACK");
  if (selectedMenuItem == 2) {
    drawDashedRect(35, 46, 61, 10, animationFrame / 5, WHITE);
  }

  tinyfont.setCursor(38, 58);
  tinyfont.print("VS ");
  tinyfont.print(opponentModeLabel());
  if (selectedMenuItem == 3) {
    drawDashedRect(35, 55, 61, 9, animationFrame / 5, WHITE);
  }
  if (audioOsdFrames > 0) {
    arduboy.fillRect(0, 57, 44, 7, BLACK);
    tinyfont.setCursor(1, 58);
    tinyfont.print(audioModeLabel());
  }
#if defined(ALL_MENS_MORRIS_FXC_LINK) && defined(ALL_MENS_MORRIS_DEBUG)
  tinyfont.setCursor(102, 24);
  switch (linkStatus()) {
    case LINK_STATUS_PROTOCOL:
      tinyfont.print("LINK");
      break;
    case LINK_STATUS_FLIPPED:
      tinyfont.print("FLIP");
      break;
    case LINK_STATUS_I2C:
      tinyfont.print("I2C");
      break;
    default:
      tinyfont.print("CBL --");
      break;
  }
#endif
}

void drawQuickMenu() {
#ifdef ALL_MENS_MORRIS_DEBUG
  drawCenteredPanel(19, 12, 90, 42);
  tinyfont.setCursor(43, 16);
#else
  drawCenteredPanel(19, 15, 90, 35);
  tinyfont.setCursor(43, 19);
#endif
  tinyfont.print("QUICK");
#ifdef ALL_MENS_MORRIS_DEBUG
  tinyfont.setCursor(27, 24);
  tinyfont.print("UP DEBUG");
#else
  tinyfont.setCursor(27, 27);
  tinyfont.print("LEFT REWIND");
#endif
#ifdef ALL_MENS_MORRIS_DEBUG
  tinyfont.setCursor(27, 31);
  tinyfont.print("LEFT REWIND");
#else
  tinyfont.setCursor(27, 34);
  tinyfont.print("DOWN MENU");
#endif
#ifdef ALL_MENS_MORRIS_DEBUG
  tinyfont.setCursor(27, 38);
  tinyfont.print("DOWN MENU");
#else
  tinyfont.setCursor(27, 41);
  tinyfont.print("RIGHT REDO");
#endif
#ifdef ALL_MENS_MORRIS_DEBUG
  tinyfont.setCursor(27, 45);
  tinyfont.print("RIGHT REDO");
#endif
}

void drawConfirm() {
  drawCenteredPanel(18, 20, 92, 25);
  tinyfont.setCursor(32, 25);
  tinyfont.print("MAIN MENU?");
  tinyfont.setCursor(27, 36);
  tinyfont.print("B YES  LEFT NO");
}

void drawPanelTextCentered(uint8_t y, const char *text, uint8_t color) {
  tinyfont.setTextColor(color);
  tinyfont.setCursor(64 - textPixelWidth(text) / 2, y);
  tinyfont.print(text);
}

void drawPanelTextCenteredOffset(uint8_t y, const char *text, int8_t offsetX, uint8_t color) {
  tinyfont.setTextColor(color);
  tinyfont.setCursor(64 - textPixelWidth(text) / 2 + offsetX, y);
  tinyfont.print(text);
}

void drawResultPanel() {
  if (game.phase != PHASE_GAME_OVER) {
    return;
  }

  bool invert = ((animationFrame / 15) % 2) == 0;
  uint8_t bg = invert ? BLACK : WHITE;
  uint8_t fg = invert ? WHITE : BLACK;
  arduboy.fillRect(24, 18, 80, 29, bg);
  arduboy.drawRect(24, 18, 80, 29, fg);

  if (game.winner == PLAYER_NONE) {
    drawPanelTextCenteredOffset(25, "IT'S A TIE...", -4, fg);
  } else if (winnerIsCpu()) {
    drawPanelTextCenteredOffset(25, "CPU WINS!", -4, fg);
  } else if (winnerIsPlayerOneSlot()) {
    drawPanelTextCenteredOffset(25, "PLAYER 1", -4, fg);
    drawPanelTextCentered(34, "WINS!", fg);
  } else {
    drawPanelTextCenteredOffset(25, "PLAYER 2", -4, fg);
    drawPanelTextCentered(34, "WINS!", fg);
  }
}

void drawGame() {
  tinyfont.setTextColor(BLACK);
  drawBoard();
  for (uint8_t i = 0; i < game.board->pointCount; i++) {
    drawPiece(i, pointAt(game, i));
  }
  drawCursor();
  drawHud();
  drawResultPanel();
  if (confirmAction != CONFIRM_NONE) {
    drawConfirm();
  } else if (arduboy.pressed(A_BUTTON)) {
    drawQuickMenu();
  }
}

void drawScene() {
  if (scene == SCENE_MAIN_MENU) {
    drawMainMenu();
    return;
  }

  drawGame();
}

void drawCurtainOverlay(uint8_t frame, bool opening) {
  uint8_t progress = (static_cast<uint16_t>(frame) * 128) / CURTAIN_FRAMES;
  if (progress > 128) {
    progress = 128;
  }
  uint8_t width = opening ? 128 - progress : progress;
  for (uint8_t bar = 0; bar < 8; bar++) {
    uint8_t y = bar * 8;
    if ((bar & 1) == 0) {
      arduboy.fillRect(0, y, width, 8, BLACK);
    } else {
      arduboy.fillRect(128 - width, y, width, 8, BLACK);
    }
  }
}

void drawTransitionOverlay() {
  if (transitionMode == TRANSITION_CURTAIN_TO_DEMO) {
    if (transitionFrame < CURTAIN_FRAMES) {
      drawCurtainOverlay(transitionFrame, false);
    } else {
      drawCurtainOverlay(transitionFrame - CURTAIN_FRAMES, true);
    }
  } else if (transitionMode == TRANSITION_CURTAIN_TO_MENU) {
    drawCurtainOverlay(transitionFrame, false);
  }
}

void drawRejectFlash() {
  if (rejectFlashFrames == 0) {
    return;
  }
  if ((rejectFlashFrames % 2) == 0) {
    for (uint16_t i = 0; i < WIDTH * HEIGHT / 8; i++) {
      arduboy.sBuffer[i] ^= 0xFF;
    }
  }
  rejectFlashFrames--;
}

void updateTransition() {
  if (transitionMode == TRANSITION_NONE) {
    return;
  }

  transitionFrame++;
  if (transitionMode == TRANSITION_CURTAIN_TO_DEMO && transitionFrame == CURTAIN_FRAMES) {
    startDemoMatch();
  } else if (transitionMode == TRANSITION_CURTAIN_TO_DEMO && transitionFrame >= CURTAIN_FRAMES * 2) {
    transitionMode = TRANSITION_NONE;
  } else if (transitionMode == TRANSITION_CURTAIN_TO_MENU && transitionFrame >= CURTAIN_FRAMES) {
    returnToMainMenu();
    transitionMode = TRANSITION_NONE;
  }
}

void updateIdleDemoTimer(bool hasInput) {
  if (transitionMode != TRANSITION_NONE) {
    return;
  }

  if (scene == SCENE_MAIN_MENU) {
    if (hasInput) {
      idleFrames = 0;
    } else if (idleFrames < MENU_IDLE_DEMO_FRAMES) {
      idleFrames++;
    } else {
      startTransition(TRANSITION_CURTAIN_TO_DEMO);
    }
  } else if (scene != SCENE_DEMO) {
    idleFrames = 0;
  }
}

void updateDemoTimer(bool hasInput) {
  if (transitionMode != TRANSITION_NONE || scene != SCENE_DEMO) {
    return;
  }

  if (hasInput) {
    startTransition(TRANSITION_CURTAIN_TO_MENU);
    return;
  }

  demoFrames++;
  if (game.phase == PHASE_GAME_OVER) {
    if (demoGameOverFrames < DEMO_WIN_EXTRA_FRAMES) {
      demoGameOverFrames++;
    } else {
      startTransition(TRANSITION_CURTAIN_TO_MENU);
    }
  } else if (demoFrames >= DEMO_TOTAL_FRAMES) {
    startTransition(TRANSITION_CURTAIN_TO_MENU);
  }
}

void handleMainMenuInput() {
  if (arduboy.justPressed(UP_BUTTON)) {
    selectedMenuItem = selectedMenuItem == 0 ? MENU_ITEM_COUNT - 1 : selectedMenuItem - 1;
    playEffect(523, 35);
  }
  if (arduboy.justPressed(DOWN_BUTTON)) {
    selectedMenuItem = selectedMenuItem == MENU_ITEM_COUNT - 1 ? 0 : selectedMenuItem + 1;
    playEffect(392, 35);
  }
  if (arduboy.justPressed(LEFT_BUTTON)) {
    if (selectedMenuItem == 0) {
      previousBoardMenuItem();
    } else if (selectedMenuItem == 1) {
      previousRulesetMode();
    } else if (selectedMenuItem == 2) {
      firstPlayer = firstPlayer == PLAYER_ONE ? PLAYER_TWO : PLAYER_ONE;
    } else {
      previousOpponentMode();
    }
    playEffect(494, 35);
  }
  if (arduboy.justPressed(RIGHT_BUTTON)) {
    if (selectedMenuItem == 0) {
      nextBoardMenuItem();
    } else if (selectedMenuItem == 1) {
      nextRulesetMode();
    } else if (selectedMenuItem == 2) {
      firstPlayer = firstPlayer == PLAYER_ONE ? PLAYER_TWO : PLAYER_ONE;
    } else {
      nextOpponentMode();
    }
    playEffect(587, 35);
  }
  if (arduboy.justPressed(B_BUTTON)) {
    if (selectedMenuItem == 0 && selectedBoardIsPlayable()) {
      if (opponentMode == OPPONENT_LINK) {
        linkSendStart(selectedBoardMenuItem, selectedRulesetMenuItem, firstPlayer);
      }
      startMatch();
      playEffect(660, 45, 880, 70);
    } else if (selectedMenuItem == 1) {
      nextRulesetMode();
      setMessage(rulesetModeLabel());
      playEffect(784, 45);
    } else if (selectedMenuItem == 2) {
      firstPlayer = firstPlayer == PLAYER_ONE ? PLAYER_TWO : PLAYER_ONE;
      setMessage(firstPlayer == PLAYER_TWO ? "WHITE" : "BLACK");
      playEffect(784, 45);
    } else if (selectedMenuItem == 3) {
      nextOpponentMode();
      setMessage(opponentModeLabel());
      playEffect(784, 45);
    } else {
      setMessage("SOON");
      playEffect(180, 90);
    }
  }
  if (arduboy.justPressed(A_BUTTON)) {
    nextAudioMode();
    playEffect(audioMode == AUDIO_MUTED ? 160 : 784, 45);
  }
}

void handleConfirmInput() {
  if (arduboy.justPressed(B_BUTTON)) {
    if (confirmAction == CONFIRM_MAIN_MENU) {
      if (isLinkMatch()) {
        linkSendMenu();
      }
      returnToMainMenu();
      playEffect(392, 45, 262, 65);
    }
    return;
  }

  if (arduboy.justPressed(LEFT_BUTTON)) {
    confirmAction = CONFIRM_NONE;
    setMessage("");
    playEffect(220, 40);
  }
}

void handleQuickMenuInput() {
  if (!arduboy.pressed(A_BUTTON)) {
    return;
  }

  if (arduboy.justPressed(UP_BUTTON)) {
#ifdef ALL_MENS_MORRIS_DEBUG
    loadDebugScenario();
#endif
  } else if (arduboy.justPressed(LEFT_BUTTON) || arduboy.justPressed(RIGHT_BUTTON)) {
    bool redo = arduboy.justPressed(RIGHT_BUTTON);
    bool changed = redo ? redoHistory() : undoHistory();
    if (changed) {
      if (isLinkMatch()) {
        linkSendAction(redo ? TURN_ACTION_MOVE : TURN_ACTION_PLACE, NO_POINT, NO_POINT);
      }
      setMessage(redo ? "REDO" : "REWIND");
      playEffect(redo ? 440 : 330, 70);
    } else {
      setMessage(redo ? "NO REDO" : "NO UNDO");
      playEffect(160, 80);
    }
  } else if (arduboy.justPressed(DOWN_BUTTON)) {
    confirmAction = CONFIRM_MAIN_MENU;
    setMessage("");
    playEffect(330, 35);
  }
}

void handleInput() {
  if (scene == SCENE_MAIN_MENU) {
    handleMainMenuInput();
    return;
  }

  if (scene == SCENE_DEMO) {
    return;
  }

  if (confirmAction != CONFIRM_NONE) {
    handleConfirmInput();
    return;
  }

  if (isCpuTurn()) {
    return;
  }

  if (linkInputLocked()) {
    if (arduboy.pressed(A_BUTTON) && arduboy.justPressed(DOWN_BUTTON)) {
      confirmAction = CONFIRM_MAIN_MENU;
      setMessage("");
      playEffect(330, 35);
      clearPendingCursorDirection();
      return;
    }
    if (anyButtonJustPressed()) {
      rejectInput();
    }
    clearPendingCursorDirection();
    return;
  }

  if (arduboy.pressed(A_BUTTON)) {
    clearPendingCursorDirection();
    handleQuickMenuInput();
    return;
  }

  handleCursorDirectionInput();

  if (arduboy.justPressed(B_BUTTON)) {
    clearPendingCursorDirection();
    GamePhase phaseBeforeAction = game.phase;
    MorrisGameState beforeAction = game;
    TurnActionMode actionMode = (game.phase == PHASE_MOVING
        && game.selected == NO_POINT
        && canPlaceAt(game, game.cursor))
        ? TURN_ACTION_PLACE
        : TURN_ACTION_MOVE;
    uint8_t actionFrom = game.phase == PHASE_MOVING && game.selected != NO_POINT ? game.selected : NO_POINT;
    uint8_t actionTo = game.cursor;
    bool moved = applyPrimaryAction(game);
    if (!moved) {
      setMessage("NOPE");
      playEffect(120, 80);
    } else if (gameStateChanged(beforeAction, game)) {
      pushHistory();
    }

    if (!moved) {
      return;
    } else {
      if (isLinkMatch()) {
        linkSendAction(actionMode, actionFrom, actionTo);
      }
      showActionFeedback(phaseBeforeAction, true);
      cpuThinkFrames = isCpuTurn() ? cpuThinkDelayFrames() : 0;
    }
  }
}

void playCpuCursorStepSound() {
  playEffect(740, 12);
}

void playCpuActionSound() {
  playEffect(220, 35);
}

uint8_t cpuAnimationStepCount(const AiAction &action) {
  uint8_t cursor = game.cursor;
  uint8_t steps = 0;
  if (action.from != NO_POINT) {
    steps += graphDistance(cursor, action.from);
    cursor = action.from;
  }
  steps += graphDistance(cursor, action.to);
  return steps == 0 ? 1 : steps;
}

void setCpuAnimationStepRate(const AiAction &action) {
  uint8_t steps = cpuAnimationStepCount(action);
  uint8_t pauses = action.from == NO_POINT ? CPU_ACTION_PAUSE_FRAMES : CPU_ACTION_PAUSE_FRAMES * 2;
  uint8_t moveFrames = CPU_ANIMATION_TARGET_FRAMES > pauses
      ? CPU_ANIMATION_TARGET_FRAMES - pauses
      : CPU_ANIMATION_TARGET_FRAMES;
  uint8_t frames = moveFrames / steps;
  if (frames < CPU_MIN_STEP_FRAMES) {
    frames = CPU_MIN_STEP_FRAMES;
  } else if (frames > CPU_MAX_STEP_FRAMES) {
    frames = CPU_MAX_STEP_FRAMES;
  }
  cpuAnimationStepFrames = frames;
  cpuAnimationStepTimer = 0;
}

void enterCpuPickPause() {
  game.cursor = cpuAnimationAction.from;
  game.selected = cpuAnimationAction.from;
  game.actionMode = cpuAnimationAction.mode;
  cpuAnimationPauseFrames = CPU_ACTION_PAUSE_FRAMES;
  cpuAnimationPhase = CPU_ANIM_PICK;
  playCpuActionSound();
}

void enterCpuDropPause() {
  game.cursor = cpuAnimationAction.to;
  cpuAnimationPauseFrames = CPU_ACTION_PAUSE_FRAMES;
  cpuAnimationPhase = CPU_ANIM_DROP;
  playCpuActionSound();
}

void finishCpuAnimation() {
  cpuAnimationPhase = CPU_ANIM_IDLE;
  bool moved = applyPrimaryAction(game);
  if (moved) {
    showActionFeedback(cpuAnimationPhaseBeforeAction, true);
  } else {
    setMessage(isLinkMatch() ? "LINK ERR" : "CPU WAIT");
  }
  cpuThinkFrames = isCpuTurn() ? cpuThinkDelayFrames() : 0;
}

void startCpuAnimation() {
  cpuAnimationPhaseBeforeAction = game.phase;
  MorrisGameState plannedResult;
  if (!chooseAiAction(game, selectedAiDifficulty(), cpuAnimationAction, plannedResult)) {
    setMessage("CPU WAIT");
    cpuThinkFrames = cpuThinkDelayFrames();
    return;
  }

  game.actionMode = cpuAnimationAction.mode;
  game.selected = NO_POINT;
  setCpuAnimationStepRate(cpuAnimationAction);
  if (cpuAnimationAction.from != NO_POINT) {
    cpuAnimationTarget = cpuAnimationAction.from;
    cpuAnimationPhase = CPU_ANIM_TO_FROM;
    if (game.cursor == cpuAnimationTarget) {
      enterCpuPickPause();
    }
  } else {
    cpuAnimationTarget = cpuAnimationAction.to;
    cpuAnimationPhase = CPU_ANIM_TO_TO;
    if (game.cursor == cpuAnimationTarget) {
      enterCpuDropPause();
    }
  }
}

void updateCpuMoveToTarget(CpuAnimationPhase nextPause) {
  if (game.cursor == cpuAnimationTarget) {
    if (nextPause == CPU_ANIM_PICK) {
      enterCpuPickPause();
    } else {
      enterCpuDropPause();
    }
    return;
  }

  if (cpuAnimationStepTimer > 0) {
    cpuAnimationStepTimer--;
    return;
  }

  game.cursor = nextGraphPointToward(game.cursor, cpuAnimationTarget);
  cpuAnimationStepTimer = cpuAnimationStepFrames > 0 ? cpuAnimationStepFrames - 1 : 0;
  playCpuCursorStepSound();
}

void updateCpuAnimation() {
  if (cpuAnimationPhase == CPU_ANIM_TO_FROM) {
    updateCpuMoveToTarget(CPU_ANIM_PICK);
  } else if (cpuAnimationPhase == CPU_ANIM_PICK) {
    if (cpuAnimationPauseFrames > 0) {
      cpuAnimationPauseFrames--;
      return;
    }
    cpuAnimationTarget = cpuAnimationAction.to;
    cpuAnimationPhase = CPU_ANIM_TO_TO;
  } else if (cpuAnimationPhase == CPU_ANIM_TO_TO) {
    updateCpuMoveToTarget(CPU_ANIM_DROP);
  } else if (cpuAnimationPhase == CPU_ANIM_DROP) {
    if (cpuAnimationPauseFrames > 0) {
      cpuAnimationPauseFrames--;
      return;
    }
    finishCpuAnimation();
  }
}

void updateCpuTurn() {
  if (isCpuAnimating()) {
    updateCpuAnimation();
    return;
  }

  if (!isCpuTurn()) {
    cpuThinkFrames = 0;
    return;
  }

  if (cpuThinkFrames > 0) {
    cpuThinkFrames--;
    return;
  }

  startCpuAnimation();
}
}

void gameSetup() {
  arduboy.begin();
  //arduboy.waitNoButtons();
  //ledsOff();
  arduboy.setFrameRate(GAME_FPS);
  arduboy.setTextColor(BLACK);
  arduboy.audio.begin();
  uint32_t seed = static_cast<unsigned long>(micros()) ^ analogRead(A0);
  randomSeed(seed);
  linkBegin(seed);
  tinyfont.setTextColor(BLACK);
  resetMorrisGame(game, ClassicBoardDefinition, ClassicRuleSet);
  playBootAnimation();
  menuMusicTheme = random(MENU_MUSIC_THEME_COUNT);
  restartMenuMusic();
}

void gameLoop() {
  if (!arduboy.nextFrame()) {
    return;
  }

  animationFrame++;
  arduboy.pollButtons();
  linkUpdate(linkDiscoveryMenuActive(), selectedBoardMenuItem, firstPlayer);
  handleLinkEvents();
  bool hasInput = arduboy.buttonsState() != 0;
  if (transitionMode == TRANSITION_NONE) {
    updateIdleDemoTimer(hasInput);
  }
  if (scene == SCENE_MAIN_MENU) {
    updateMenuAnimation();
  }
  if (transitionMode == TRANSITION_NONE) {
    if (scene == SCENE_DEMO) {
      updateDemoTimer(hasInput);
    } else {
      handleInput();
    }
  }
  if (transitionMode == TRANSITION_NONE) {
    updateCpuTurn();
  } else {
    updateTransition();
  }
  updateMillLed();
  updateMenuMusic();
  if (messageFrames > 0) {
    messageFrames--;
  }
  if (audioOsdFrames > 0) {
    audioOsdFrames--;
  }

  arduboy.fillScreen(WHITE);
  drawScene();
  drawTransitionOverlay();
  drawRejectFlash();
  arduboy.display();
}
