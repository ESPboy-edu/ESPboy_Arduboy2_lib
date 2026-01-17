/*Volleyball for Arduboy
*"Jet Suricat" Konstantin Tor (melanjet)
*/

#include <Arduboy2.h>
#include <ArduboyTones.h>
//#include <EEPROM.h>

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);

// --- ДАННЫЕ СПРАЙТОВ ---

// МЯЧ (8x8)
constexpr uint8_t BallWidth { 8 };
constexpr uint8_t BallHeight { 8 };

constexpr uint8_t BallSprite[] PROGMEM
{
    // Width, Height
    0x3C, 0x4E, 0x9B, 0xE5, 0x93, 0xE5, 0x52, 0x3C,
    0x3C, 0x4E, 0xA3, 0xB1, 0xDD, 0xC1, 0x4E, 0x3C,
    0x3C, 0x66, 0xD9, 0xA3, 0xDD, 0xA3, 0x5E, 0x3C,
};
#define BALL_FRAME_SIZE (8 * 1)
#define BALL_FRAME_COUNT 3


// ИГРОК (10x10) - Player (p1)
constexpr uint8_t PlayerWidth { 10 };
constexpr uint8_t PlayerHeight { 10 };

constexpr uint8_t PlayerSprite[] PROGMEM
{
    0x7C, 0xFE, 0xDF, 0x93, 0xB3, 0xBF, 0xBF, 0xBE, 0xDC, 0x18, 0x00, 0x00, 0x00, 0x03, 0x03, 0x02, 0x02, 0x00, 0x00, 0x00,
    0x7C, 0xFE, 0xFF, 0xF3, 0x93, 0x9F, 0xBF, 0xBE, 0xDC, 0x18, 0x00, 0x00, 0x00, 0x03, 0x02, 0x02, 0x03, 0x02, 0x02, 0x00,
    0x7C, 0xFE, 0xFF, 0xB3, 0xB3, 0xBF, 0xBF, 0xBE, 0xDC, 0x18, 0x00, 0x00, 0x03, 0x02, 0x02, 0x03, 0x02, 0x02, 0x00, 0x00,
};
#define PLAYER_FRAME_SIZE (10 * 2)
#define PLAYER_FRAME_COUNT 3


// CPU (10x10) - Player1 (p2)
constexpr uint8_t Player1Sprite[] PROGMEM
{
    0x18, 0xDC, 0xBE, 0xBF, 0xBF, 0xB3, 0x93, 0xDF, 0xFE, 0x7C, 0x00, 0x00, 0x00, 0x02, 0x02, 0x03, 0x03, 0x00, 0x00, 0x00,
    0x18, 0xDC, 0xBE, 0xBF, 0x9F, 0x93, 0xF3, 0xFF, 0xFE, 0x7C, 0x00, 0x02, 0x02, 0x03, 0x02, 0x02, 0x03, 0x00, 0x00, 0x00,
    0x18, 0xDC, 0xFE, 0xBF, 0xFF, 0xF3, 0xF3, 0xFF, 0xFE, 0x7C, 0x00, 0x00, 0x02, 0x02, 0x03, 0x02, 0x02, 0x03, 0x00, 0x00,
};


// --- ГЛОБАЛЬНЫЕ КОНСТАНТЫ ИГРЫ ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define PLAYER_WIDTH PlayerWidth // 10
#define PLAYER_HEIGHT PlayerHeight // 10
#define PLAYER_MOVE_SPEED 1.5f

#define PLAYER_JUMP_FORCE -3.8f
#define PLAYER_GRAVITY 0.3f

#define BALL_SIZE BallWidth // 8
#define BALL_GRAVITY 0.12f

#define NET_X (SCREEN_WIDTH / 2 - 1)
#define NET_HEIGHT 25
#define NET_Y (SCREEN_HEIGHT - NET_HEIGHT)

#define FLOOR_Y (SCREEN_HEIGHT - 1)

// Состояния
#define STATE_TITLE 0
#define STATE_SERVE 1
#define STATE_PLAY 2
#define STATE_POINT_SCORED 3
#define STATE_GAME_OVER 4
#define STATE_PAUSED 5
#define STATE_STATS 6
#define STATE_BEACH_SELECT 7 

// Стороны
#define SIDE_PLAYER 0
#define SIDE_CPU 1

// --- КОНСТАНТЫ ЧЕМПИОНАТА ---
#define BEACH_SANTA_MONICA 0
#define BEACH_LONG_BEACH 1
#define BEACH_PACIFIC_BEACH 2
#define BEACH_MALIBU 3
#define TOTAL_MATCHES_PER_BEACH 4
#define TOTAL_BEACHES 4

// --- КОНСТАНТЫ ИИ ---
#define CPU_DEFENSIVE_CENTER 95.0f
#define CPU_DEFENSIVE_BACK_CORNER 110.0f // (95 + 15)

// --- Структуры данных ---
struct Entity {
  float x, y;
  float vx, vy;
};

struct Player : Entity {
  bool onGround;
  int hits;
  int score;
  int hitCooldown = 0; // Кулдаун касания
};

// --- Глобальные переменные ---
Player p1;
Player p2;
Entity ball;

int gameState;
int serverTurn;
int pointDelayTimer;
int cpuJumpCooldown = 0;
int ballSide;
bool statsSavedForMatch = false; 
bool audioEnabled = true; 
int soundIndicatorTimer = 0; 

int frameCounter = 0;
const int ANIMATION_SPEED = 4;

float cpuMoveSpeed = PLAYER_MOVE_SPEED; // Скорость ИИ

// --- ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ ДЛЯ СТАТИСТИКИ И ПРОГРЕССА ---
int totalWins = 0; 
int totalGames = 0; 
int currentBeach = BEACH_SANTA_MONICA; // Текущий пляж
int matchWins = 0; // Победы на текущем пляже (нужно 4)
bool isNewMatch = true; // Флаг, который показывает, что матч только начался

bool DEBUG_MODE = false; 
// --- НОВЫЕ ПЕРЕМЕННЫЕ ДЛЯ РЕЖИМА ШОУ (CPU TAUNTS) ---
const int TAUNT_DURATION = 40; // Длительность показа сообщения в кадрах 
int tauntTimer = 0;           // Таймер для контроля показа сообщения
int tauntCooldown = 0;        // Кулердаун перед следующим сообщением (случайное значение)
int currentTauntIndex = 0;    // Индекс текущего сообщения для показа
bool tauntEnabled = false;    // Флаг, который показывает, что режим шоу включен

// --- СООБЩЕНИЯ CPU ДЛЯ РЕЖИМА "ШОУ"  ---
const uint8_t TauntIDs[] PROGMEM = {
    1, // EASY!
    2, // LAME!
    3, // HA-HA!
    4, // NOPE
    5, // :(
    6, // :P
    7, // BYE!
    8, // FAST!
    9, // GIFT!
    10, // ROOF!
    11, // BOOM!
    12, // SEE?
    13, // WHIFF!
    14, // OUT!
    15, // MISS!
    16, // SHANK!
    17, // OOPS!
    18, // NOOB!
    19, // THUMBS!
    20, // WEAK!
    21, // LOOK!
    22, // SHORT!
    23, // NO WAY!
    24, // MORE!
    25, // POOP!
    26, // OH...NO!
    27, // No Touch!
    28 //ONE-NIL!
};
const int TOTAL_TAUNTS = 28;
// Переменные для шоу теперь хранят только ID
uint8_t currentTauntID = 0; 

// ---  ПЕРЕМЕННЫЕ ИИ И МЕНЮ ---
float cpuServeOffset = 0; // Смещение для подачи
float cpuPatrolTargetX = CPU_DEFENSIVE_CENTER; // Цель для патрулирования
bool isSingleMatchMode = false; // true = Тренировка, false = Тур
int titleMenuSelection = 0; // 0 = Tour, 1 = Single
int currentMatchDifficulty; // Определяет скорость ИИ, 

// --- АДРЕСА EEPROM ---
#define EEPROM_ADDRESS_WINS 100
#define EEPROM_ADDRESS_GAMES 102
#define EEPROM_ADDRESS_CHECK 104
#define EEPROM_ADDRESS_BEACH 106  // Текущий пляж (2 байта)
#define EEPROM_ADDRESS_MATCH_WINS 108 // Победы на текущем пляже (2 байта)
#define EEPROM_CHECK_VALUE 0xAA 

// --- Прототипы функций ---
void loadStats();
void saveStats();
void resetGame();
void resetRound();
void updateGame();
void drawGame();
void updatePlayer(Player* p);
void handleInput();
void updateCPU();
void updateCPUPlayer1();
void updateBall();
bool checkCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
void checkAllCollisions();
void handleBallPlayerCollision(Player* p);
void checkScoring();
void startNextRound();
void handleRallyEnd(int winner); 
void handlePointEnd(int sideThatLost);


// --- Инициализация ---

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(40);
  arduboy.audio.on();
  
  randomSeed(analogRead(A0));
  
  gameState = STATE_TITLE;
  loadStats();
  resetGame(); 
}

void resetGame() {
  statsSavedForMatch = false; 
  
  cpuMoveSpeed = PLAYER_MOVE_SPEED; 

  p1.score = 0;
  p2.score = 0;
  serverTurn = SIDE_PLAYER;
  
  // *** Сбрасываем флаг Одиночного матча ***
  isSingleMatchMode = false; 

   
  if (currentBeach < 0 || currentBeach >= TOTAL_BEACHES) currentBeach = BEACH_SANTA_MONICA;
  if (matchWins < 0 || matchWins > TOTAL_MATCHES_PER_BEACH) matchWins = 0;
  
  isNewMatch = true;
  
  if (gameState == STATE_GAME_OVER) { 
      gameState = STATE_TITLE; 
  }
  
  if (gameState == STATE_TITLE) {
      resetRound();
      p1.hits = 0; p2.hits = 0;
      ball.vx = 1.0f;
      ball.vy = -1.0f;
  } else {
      resetRound();
  }
}

void resetRound() {
  if (gameState != STATE_TITLE && gameState != STATE_GAME_OVER && gameState != STATE_BEACH_SELECT) { 
      gameState = STATE_SERVE;
  }
  
  pointDelayTimer = 90;

  p1.x = 30;
  p1.y = FLOOR_Y - PLAYER_HEIGHT;
  p1.vx = 0; p1.vy = 0;
  p1.onGround = true; p1.hits = 0; p1.hitCooldown = 0; 
  
  p2.x = SCREEN_WIDTH - 30 - PLAYER_WIDTH;
  p2.y = FLOOR_Y - PLAYER_HEIGHT;
  p2.vx = 0; p2.vy = 0;
  p2.onGround = true; p2.hits = 0; p2.hitCooldown = 0; 

  if (serverTurn == SIDE_PLAYER) {
    ball.x = p1.x + PLAYER_WIDTH / 2;
    ball.y = 30;
    ballSide = SIDE_PLAYER;
  } else {
    ball.x = p2.x - PLAYER_WIDTH / 2;
    ball.y = 30;
    ballSide = SIDE_CPU;
    
    cpuServeOffset = random(-5, 6); 
  }
  
  if (gameState != STATE_TITLE) {
      ball.vx = 0; ball.vy = 0;
  }
}

// --- УПРАВЛЕНИЕ EEPROM ---

void loadStats() {
    uint8_t check;
    EEPROM.get(EEPROM_ADDRESS_CHECK, check);
    
    if (check != EEPROM_CHECK_VALUE) {
        totalWins = 0;
        totalGames = 0;
        currentBeach = BEACH_SANTA_MONICA; 
        matchWins = 0; 
        saveStats();
        EEPROM.put(EEPROM_ADDRESS_CHECK, EEPROM_CHECK_VALUE);
        EEPROM.commit();
    } else {
        EEPROM.get(EEPROM_ADDRESS_WINS, totalWins);
        EEPROM.get(EEPROM_ADDRESS_GAMES, totalGames);
        EEPROM.get(EEPROM_ADDRESS_BEACH, currentBeach); 
        EEPROM.get(EEPROM_ADDRESS_MATCH_WINS, matchWins); 
        
        if (currentBeach < 0 || currentBeach >= TOTAL_BEACHES) currentBeach = BEACH_SANTA_MONICA;
        if (matchWins < 0 || matchWins > TOTAL_MATCHES_PER_BEACH) matchWins = 0;
    }
}

void saveStats() {
    EEPROM.put(EEPROM_ADDRESS_WINS, totalWins); 
    EEPROM.put(EEPROM_ADDRESS_GAMES, totalGames);
    EEPROM.put(EEPROM_ADDRESS_BEACH, currentBeach); 
    EEPROM.put(EEPROM_ADDRESS_MATCH_WINS, matchWins); 
    EEPROM.commit();
}

// --- Основной цикл ---

void loop() {
  if (!(arduboy.nextFrame())) {
    return;
  }
  arduboy.pollButtons();

  updateGame();
  drawGame();

  arduboy.display();
}

// --- Обновление логики ---

void updateGame() {
  frameCounter++;
  if (frameCounter >= ANIMATION_SPEED * PLAYER_FRAME_COUNT) {
      frameCounter = 0;
  }

  // --- УПРАВЛЕНИЕ АДАПТИВНОЙ СКОРОСТЬЮ ПО ПЛЯЖУ (СЛОЖНОСТЬ) ---
  if (currentMatchDifficulty == BEACH_SANTA_MONICA) { 
      cpuMoveSpeed = 1.5f;
  } else if (currentMatchDifficulty == BEACH_MALIBU) { 
      cpuMoveSpeed = 2.2f;
  } else { 
      if (p1.score >= 7) {
          cpuMoveSpeed = 2.2f; 
      } else if (p1.score >= 5) {
          cpuMoveSpeed = 1.8f; 
      } else {
          cpuMoveSpeed = 1.5f; 
      }
  }
// Включается, если прогресс Тура > Santa Monica, не Single Match и не Title Screen.
    tauntEnabled = (currentBeach > BEACH_SANTA_MONICA) && !isSingleMatchMode && (gameState != STATE_TITLE); 


    
  /*if (arduboy.justPressed(UP_BUTTON)) {
      audioEnabled = !audioEnabled; 
      
      if (audioEnabled) {
          arduboy.audio.on(); 
      } else {
          arduboy.audio.off(); 
      }
      
      soundIndicatorTimer = 120;
  }
  
  if (soundIndicatorTimer > 0) {
      soundIndicatorTimer--;
  }
*/
  // --- ЛОГИКА ДЕБАГ МОДА: ВКЛЮЧЕНИЕ / ВЫКЛЮЧЕНИЕ (LEFT + UP) ---
  if (gameState == STATE_TITLE || gameState == STATE_BEACH_SELECT) {
      if (arduboy.pressed(LEFT_BUTTON) && arduboy.justPressed(UP_BUTTON)) {
          DEBUG_MODE = !DEBUG_MODE;
          if (DEBUG_MODE) sound.tone(1000, 100);
          else sound.tone(500, 100);
      }
  }
  
  // --- ЛОГИКА ДЕБАГ МОДА: НАВИГАЦИЯ ПО ПЛЯЖАМ (LEFT / RIGHT) ---
if (DEBUG_MODE && (gameState == STATE_TITLE || gameState == STATE_BEACH_SELECT)) {
    
    bool changed = false; // Флаг для отслеживания изменений
    
    if (arduboy.justPressed(RIGHT_BUTTON)) {
        currentBeach = (currentBeach + 1) % TOTAL_BEACHES;
        matchWins = 0; 
        changed = true;
        sound.tone(880, 50);
    }
    if (arduboy.justPressed(LEFT_BUTTON)) {
        currentBeach = (currentBeach - 1 + TOTAL_BEACHES) % TOTAL_BEACHES;
        matchWins = 0; 
        changed = true;
        sound.tone(880, 50);
    }
    
    if (changed) {
                
        // 2. Сбрасываем флаг Single Match (чтобы Show Mode включился)
        
        isSingleMatchMode = false; // 
        // **********************************
        
        // 3. ПРИНУДИТЕЛЬНО обновляем currentMatchDifficulty для ИИ 
        currentMatchDifficulty = currentBeach;

        // 4. ПРИНУДИТЕЛЬНО обновляем флаг "Шоу"
        tauntEnabled = (currentBeach > BEACH_SANTA_MONICA);
    }
}

  if (arduboy.justPressed(B_BUTTON)) {
      if (gameState == STATE_PLAY || gameState == STATE_SERVE) {
          gameState = STATE_PAUSED;
          //arduboy.audio.off(); 
          return; 
      } else if (gameState == STATE_PAUSED) {
          gameState = STATE_PLAY; 
          //arduboy.audio.on(); 
      }
  }

  if (gameState != STATE_TITLE && gameState != STATE_PAUSED) {
    handleInput();
  }

  switch (gameState) {
    case STATE_TITLE:
        //выключение звука в игре
        if (arduboy.justPressed(LEFT_BUTTON)) {
      audioEnabled = !audioEnabled; 
      
      if (audioEnabled) {
          arduboy.audio.on(); 
      } else {
          arduboy.audio.off(); 
      }
      
      soundIndicatorTimer = 10;
  }
  
  if (soundIndicatorTimer > 0) {
      soundIndicatorTimer--;
  }
        
        // ***  ЛОГИКА МЕНЮ (UP/DOWN/A) ***
        if (arduboy.justPressed(UP_BUTTON)) {
            titleMenuSelection = 0; // Tour
            sound.tone(600, 20);
        }
        if (arduboy.justPressed(DOWN_BUTTON)) {
            titleMenuSelection = 1; // Single
            sound.tone(600, 20);
        }

        if (arduboy.justPressed(A_BUTTON)) {
            if (titleMenuSelection == 0) { // 0 = Tour
                isSingleMatchMode = false;
                loadStats(); // 
                currentMatchDifficulty = currentBeach;

                gameState = STATE_BEACH_SELECT; 
                resetRound();
                return;
            } else { // 1 = Single Match
                isSingleMatchMode = true;
                
                currentMatchDifficulty = BEACH_SANTA_MONICA; 
                gameState = STATE_SERVE;
                p1.score = 0;
                p2.score = 0;
                serverTurn = SIDE_PLAYER;
                resetRound(); 
                return;
            }
        }
        if (arduboy.justPressed(B_BUTTON)) {
            gameState = STATE_STATS;
            return;
        }

        // Логика демо-игры CPU vs CPU (продолжает работать)
        updateCPUPlayer1();
        updateCPU();
        updateBall();
        checkAllCollisions();
        
        if (ball.y + BALL_SIZE >= FLOOR_Y || p1.hits >= 4 || p2.hits >= 4) {
            ball.x = random(SCREEN_WIDTH / 2 - 10, SCREEN_WIDTH / 2 + 10);
            ball.y = 10;
            ball.vy = -1.0f;
            ball.vx = (random(0, 2) == 0) ? 1.5f : -1.5f;
            p1.hits = 0; p2.hits = 0;
            if (arduboy.frameCount % 120 == 0) {
                resetRound();
                ball.vx = (random(0, 2) == 0) ? 1.5f : -1.5f;
                ball.vy = -1.0f;
            }
        }
        break;
        
    case STATE_BEACH_SELECT: 
        if (arduboy.justPressed(A_BUTTON)) {
            // Подтверждение и старт игры
            
            if (tauntEnabled) {
                tauntCooldown = 1; // Запускаем первый кулдаун
            }
            gameState = STATE_SERVE;
            p1.score = 0;
            p2.score = 0;
            serverTurn = SIDE_PLAYER;
            resetRound(); 
            return;
        }
        if (arduboy.justPressed(B_BUTTON)) {
            // Возврат на титульный экран
            gameState = STATE_TITLE;
            return;
        }
        break;


    case STATE_SERVE:
      updatePlayer(&p1);
      updateCPU();

      if (checkCollision(ball.x, ball.y, BALL_SIZE, BALL_SIZE, p1.x, p1.y, PLAYER_WIDTH, PLAYER_HEIGHT) && serverTurn == SIDE_PLAYER) {
        handleBallPlayerCollision(&p1);
        gameState = STATE_PLAY;
      }
      
      else if (serverTurn == SIDE_CPU) {
        pointDelayTimer--;
        
        if (checkCollision(ball.x, ball.y, BALL_SIZE, BALL_SIZE, p2.x, p2.y, PLAYER_WIDTH, PLAYER_HEIGHT)) {
            handleBallPlayerCollision(&p2);
            gameState = STATE_PLAY;
        }
      }
      break;

    case STATE_PLAY:
      updatePlayer(&p1);
      updateCPU();
      updateBall();
      checkAllCollisions();
      checkScoring();
      // --- ЛОГИКА УПРАВЛЕНИЯ ТАЙМЕРАМИ РЕЖИМА "ШОУ"  ---
      if (tauntEnabled) { 
          if (tauntTimer > 0) {
              tauntTimer--; // Отсчет времени показа
          } else if (tauntCooldown > 0) {
              tauntCooldown--; // Отсчет кулдауна
          } else {
              // Кулдаун закончился, пора показать сообщение
              currentTauntID = random(1,TOTAL_TAUNTS + 1); 
              tauntTimer = TAUNT_DURATION; // Запускаем таймер показа
              
              // Если это первый запуск, или обычный кулдаун, устанавливаем следующий
              tauntCooldown = random(200, 400); 
          }
      }
      break;
      
      
    case STATE_PAUSED: 
      break;
      

    case STATE_POINT_SCORED:
      pointDelayTimer--;
      if (pointDelayTimer <= 0) {
        resetRound();
      }
      break;

    case STATE_GAME_OVER:
      // Логика сохранения статистики
      if (!statsSavedForMatch) {
          totalGames++;
          if (p1.score > p2.score) {
              totalWins++;
          }
          saveStats(); // Сохраняем только если это был матч Тура
          statsSavedForMatch = true;
      }
      
      // Блок проверки нажатия кнопок
     // 2. Логика сохранения/сброса прогресса Тура (выполняется при нажатии кнопки)
    if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) {
        
        // --- Логика для режима Тура ---
        if (!isSingleMatchMode) {
            
            // Если игрок выиграл матч
            if (p1.score > p2.score) {
                
                
                if (currentBeach == BEACH_MALIBU && matchWins >= TOTAL_MATCHES_PER_BEACH) {
                     // Чемпионат выигран, сбрасываем Тур на старт
                     currentBeach = BEACH_SANTA_MONICA;
                     matchWins = 0;
                }
                
                
                saveStats(); 
            } 
            
            // Если игрок проиграл матч (p1.score <= p2.score)
            else { 
                // *** ПОЛНЫЙ СБРОС ТУРА ***
                currentBeach = BEACH_SANTA_MONICA; // Возврат на первый пляж
                matchWins = 0;                      // Сброс побед
                saveStats();                      // Сохраняем ПОЛНЫЙ сброс
            }
        }
        
        resetGame(); // Сбрасываем счет и переходим на STATE_TITLE
    }
    break;
     
    case STATE_STATS: 
        if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) {
            gameState = STATE_TITLE; 
            //arduboy.audio.on();
            resetGame(); 
            return; 
        }
    if (arduboy.pressed(DOWN_BUTTON)) {
        totalWins = 0;
        totalGames = 0;
        currentBeach = BEACH_SANTA_MONICA;
        matchWins = 0;
        saveStats(); 
        sound.tone(880, 100); 
    }
        break;
  }
    
}

void updatePlayer(Player* p) {
  p->vy += PLAYER_GRAVITY;
  p->y += p->vy;

  if (p->y + PLAYER_HEIGHT >= FLOOR_Y) {
    p->y = FLOOR_Y - PLAYER_HEIGHT;
    p->vy = 0;
    p->onGround = true;
  }
  
  if (p->hitCooldown > 0) p->hitCooldown--; 

  p->x += p->vx;

  if ((int)p == (int)&p1) {
    if (p->x < 1) p->x = 1;
    if (p->x + PLAYER_WIDTH > NET_X) p->x = NET_X - PLAYER_WIDTH;
  } else {
    if (p->x < NET_X + 2) p->x = NET_X + 2;
    if (p->x + PLAYER_WIDTH > SCREEN_WIDTH - 1) p->x = SCREEN_WIDTH - 1 - PLAYER_WIDTH;
  }
  p->vx = 0;
}

void handleInput() {
  if (arduboy.pressed(LEFT_BUTTON)) {
    p1.vx = -PLAYER_MOVE_SPEED;
  }
  if (arduboy.pressed(RIGHT_BUTTON)) {
    p1.vx = PLAYER_MOVE_SPEED;
  }
  if (arduboy.justPressed(A_BUTTON) && p1.onGround) {
    p1.vy = PLAYER_JUMP_FORCE;
    p1.onGround = false;
  }
  if (gameState == STATE_SERVE && serverTurn == SIDE_PLAYER && arduboy.justPressed(A_BUTTON) && p1.onGround) {
    p1.vy = PLAYER_JUMP_FORCE;
    p1.onGround = false;
  }
}

void updateCPUPlayer1() {
  float ballCenter = ball.x + BALL_SIZE / 2;
  float p1Center = p1.x + PLAYER_WIDTH / 2;

  if (p1Center < ballCenter - 4) {
    p1.vx = PLAYER_MOVE_SPEED;
  } else if (p1Center > ballCenter + 4) {
    p1.vx = -PLAYER_MOVE_SPEED;
  }

  bool ballOnP1Side = ball.x < NET_X;
  bool ballIsDropping = ball.vy > 0.3f;
  
  if (p1.onGround && ballOnP1Side && ballIsDropping && ball.y > 25) {
      if (abs(ballCenter - p1Center) < PLAYER_WIDTH / 2) {
          p1.vy = PLAYER_JUMP_FORCE;
          p1.onGround = false;
      }
  }

  updatePlayer(&p1);
}


//  ДИНАМИЧЕСКИЙ ИИ (Прыжки, Подача, Патрулирование) ***
void updateCPU() {
  float ballCenter = ball.x + BALL_SIZE / 2;
  float cpuCenter = p2.x + PLAYER_WIDTH / 2;

  if (cpuJumpCooldown > 0) {
      cpuJumpCooldown--;
  }

  if (gameState == STATE_SERVE && serverTurn == SIDE_CPU && pointDelayTimer < 20) {
      
      //  Используем targetX со смещением ***
      float targetX = ballCenter + cpuServeOffset;

      if (cpuCenter < targetX - 4) {
        p2.vx = cpuMoveSpeed;
      } else if (cpuCenter > targetX + 4) {
        p2.vx = -cpuMoveSpeed;
      }
      
      if (p2.onGround && abs(cpuCenter - targetX) < 8) { // Проверка по targetX
          if (random(0, 100) < 75) { 
              p2.vy = PLAYER_JUMP_FORCE;
              p2.onGround = false;
              cpuJumpCooldown = random(30, 50); 
          }
      }
    
  } else if (gameState == STATE_PLAY || gameState == STATE_TITLE) {
    
    bool ballOnPlayersSide = ball.x < NET_X; 

    if (ballOnPlayersSide) {
        //  ЛОГИКА ПАТРУЛИРОВАНИЯ ***
        if (abs(cpuCenter - cpuPatrolTargetX) < 1.0f) {
          // Мы достигли цели, выбираем новую
          if (cpuPatrolTargetX == CPU_DEFENSIVE_CENTER) {
              cpuPatrolTargetX = CPU_DEFENSIVE_BACK_CORNER; // Двигаемся в угол
          } else {
              cpuPatrolTargetX = CPU_DEFENSIVE_CENTER; // Возвращаемся в центр
          }
        }
        
        // Медленно движемся к цели патрулирования
        if (cpuCenter < cpuPatrolTargetX - 1.0f) {
            p2.vx = cpuMoveSpeed * 0.6f; // Медленный дрейф
        } else if (cpuCenter > cpuPatrolTargetX + 1.0f) {
            p2.vx = -cpuMoveSpeed * 0.6f;
        } else {
            p2.vx = 0; 
        }
        
    } else {
        // Мяч на стороне ИИ (Атака)
        
        // Сбрасываем цель патрулирования, чтобы ИИ был готов к обороне, когда мяч вернется
        cpuPatrolTargetX = CPU_DEFENSIVE_CENTER; 
        
        if (cpuCenter < ballCenter - 4) {
            p2.vx = cpuMoveSpeed;
        } else if (cpuCenter > ballCenter + 4) {
            p2.vx = -cpuMoveSpeed;
        }
        
        // --- ДИНАМИЧЕСКАЯ ЛОГИКА ПРЫЖКА  ---
        bool ballOnCPUSide = ball.x > NET_X + 2;
        bool ballIsDropping = ball.vy > 0.3f;
        bool canJump = p2.onGround && cpuJumpCooldown == 0;
        
        if (canJump && ballOnCPUSide && ballIsDropping) {

            // 1. Устанавливаем сложность в зависимости от пляжа
            int jumpErrorChance = 10; // 10% (Santa Monica)
            int jumpAggressionChance = 5; // 5/10 = 50% (Santa Monica)

            // В ОДИНОЧНОМ МАТЧЕ (isSingleMatchMode) используется сложность Santa Monica
            if (!isSingleMatchMode) {
                if (currentBeach == BEACH_LONG_BEACH) {
                    jumpErrorChance = 5; // 5%
                    jumpAggressionChance = 7; // 70%
                } else if (currentBeach == BEACH_PACIFIC_BEACH) {
                    jumpErrorChance = 2; // 2%
                    jumpAggressionChance = 9; // 90%
                } else if (currentBeach == BEACH_MALIBU) {
                    jumpErrorChance = 0; // 0%
                    jumpAggressionChance = 10; // 100%
                }
            }

            // 2. Применяем логику
            if (random(0, 100) < jumpErrorChance) { 
                cpuJumpCooldown = 15; 
            } 
            else {
                bool isUnderBall = abs(ballCenter - cpuCenter) < 9;
                if (isUnderBall) {
                    if (ball.y > 40) {
                        if (random(0, 10) < 9) { // Высокие мячи всегда 90%
                            p2.vy = PLAYER_JUMP_FORCE;
                            p2.onGround = false;
                            cpuJumpCooldown = 40; 
                        }
                    } 
                    else if (ball.y > 25) { 
                        if (random(0, 10) < jumpAggressionChance) { // Динамическая агрессия
                            p2.vy = PLAYER_JUMP_FORCE;
                            p2.onGround = false;
                            cpuJumpCooldown = 40;
                        }
                    }
                }
            } 
        }
    }
  }

  updatePlayer(&p2);
}

void updateBall() {
  ball.vy += BALL_GRAVITY;
  ball.x += ball.vx;
  ball.y += ball.vy;

  int currentBallSide;

  if (ball.x + BALL_SIZE / 2 < NET_X) {
      currentBallSide = SIDE_PLAYER;
  } else {
      currentBallSide = SIDE_CPU;
  }

  if (currentBallSide != ballSide) {
      
      if (gameState != STATE_TITLE) {
          if (currentBallSide == SIDE_PLAYER) {
              p1.hits = 0;
          } else {
              p2.hits = 0;
          }
      }
      
      ballSide = currentBallSide;
  }
  
  if (ball.x < 1) {
    float noiseY = random(-3, 4) / 10.0f;
    ball.vy += noiseY;
    
    ball.x = 1;
    ball.vx *= -0.7f;
  }
  if (ball.x + BALL_SIZE > SCREEN_WIDTH - 1) {
    float noiseY = random(-3, 4) / 10.0f;
    ball.vy += noiseY;
    
    ball.x = SCREEN_WIDTH - 1 - BALL_SIZE;
    ball.vx *= -0.7f;
  }
  
  if (ball.y < 1) {
    float noiseX = random(-3, 4) / 10.0f;
    ball.vx += noiseX;
    
    ball.y = 1;
    ball.vy *= -0.7f;
  }
}

// --- Столкновения и Правила ---

bool checkCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
  return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}

void checkAllCollisions() {
  
  if (checkCollision(ball.x, ball.y, BALL_SIZE, BALL_SIZE, p1.x, p1.y, PLAYER_WIDTH, PLAYER_HEIGHT)) {
    handleBallPlayerCollision(&p1);
    if (gameState != STATE_TITLE) {
      if (p1.hitCooldown == 0) { 
          p1.hits++;
          p2.hits = 0;
          p1.hitCooldown = 5; 
      }
    }
  }

  if (checkCollision(ball.x, ball.y, BALL_SIZE, BALL_SIZE, p2.x, p2.y, PLAYER_WIDTH, PLAYER_HEIGHT)) {
    handleBallPlayerCollision(&p2);
    if (gameState != STATE_TITLE) {
      if (p2.hitCooldown == 0) { 
          if (ball.x > NET_X) p2.hits++;
          p1.hits = 0;
          p2.hitCooldown = 5; 
      }
    }
  }
  
  if (checkCollision(ball.x, ball.y, BALL_SIZE, BALL_SIZE, NET_X, NET_Y, 2, 2)) {
      
      if (ball.vy > 0 && ball.y + BALL_SIZE - ball.vy <= NET_Y) {
          
          ball.vy *= -0.7f;
          ball.y = NET_Y - BALL_SIZE - 0.1f;
          
          return;
      }
  }

  if (checkCollision(ball.x, ball.y, BALL_SIZE, BALL_SIZE, NET_X, NET_Y, 2, NET_HEIGHT)) {
  
    ball.vy *= 0.4f;
    ball.vx *= -1.0f;
    
    if (ball.vx > 0) {
      ball.x = NET_X + 2 + 0.5f;
    } else {
      ball.x = NET_X - BALL_SIZE - 0.5f;
    }
  }
}

// ***  ДИНАМИЧЕСКИЙ ИИ (Удары) ***
void handleBallPlayerCollision(Player* p) {
  const float P1_BASE_SPEED = 2.8f;
  const float P2_BASE_SPEED = 3.2f; 
  const float JUMP_BOOST = 1.2f;

  float hitPos = (ball.x + BALL_SIZE / 2) - (p->x + PLAYER_WIDTH / 2);
  float hitFactor = hitPos / (PLAYER_WIDTH / 2);
  
  // --- ДИНАМИЧЕСКАЯ ТОЧНОСТЬ (ШУМ) ---
  int noiseAmount = 50; // ±0.5 (Santa Monica / Single Match)
  int strategyChance = 7; // 70% (Santa Monica / Single Match)

  if ((int)p == (int)&p2) {
    if (!isSingleMatchMode) { 
      if (currentBeach == BEACH_LONG_BEACH) {
          noiseAmount = 40; // ±0.4
          strategyChance = 8; // 80%
      } else if (currentBeach == BEACH_PACIFIC_BEACH) {
          noiseAmount = 25; // ±0.25
          strategyChance = 9; // 90%
      } else if (currentBeach == BEACH_MALIBU) {
          noiseAmount = 15; // ±0.15
          strategyChance = 10; // 100%
      }
    }
  }

  float randomNoiseX = (int)p == (int)&p2 ? random(-noiseAmount, noiseAmount + 1) / 100.0f : random(-10, 11) / 100.0f; 
  float randomNoiseY = random(-5, 6) / 10.0f;


  if ((int)p == (int)&p1) {
      // ЛОГИКА ДЛЯ ИГРОКА (P1)
      ball.vx = (hitFactor * P1_BASE_SPEED) + randomNoiseX;
      ball.vy = -P1_BASE_SPEED + randomNoiseY;
      
      if (!p->onGround) {
        ball.vy -= JUMP_BOOST;
      }
  }
  
  if ((int)p == (int)&p2) {
      // ЛОГИКА ДЛЯ ИИ (P2)
      float strategicHitFactor;
      bool playerIsNearNet = (p1.x > (NET_X / 2)); 

      if (!p->onGround) {
          // Агрессивный "Спайк" в прыжке (всегда)
          strategicHitFactor = -1.1f; 
          ball.vy = -P2_BASE_SPEED * 0.7f - JUMP_BOOST; 
      } else {
          // Удар с земли (динамическая стратегия)
          if (random(0, 10) < strategyChance) { 
              if (playerIsNearNet) {
                  strategicHitFactor = -1.0f; 
              } else {
                  strategicHitFactor = -0.5f; 
              }
          } else { 
              strategicHitFactor = -0.75f; 
          }
          ball.vy = -P2_BASE_SPEED + randomNoiseY;
      }

      // Применяем горизонтальную скорость: Стратегия + Влияние места касания + ДИНАМИЧЕСКИЙ ШУМ
      float targetVX = strategicHitFactor * P2_BASE_SPEED;
      
      targetVX += (hitFactor * P2_BASE_SPEED * 0.20f); 
      targetVX += randomNoiseX; 

      ball.vx = targetVX;
      
      if (ball.vx < -4.2f) { 
         ball.vx = -4.2f;
      }
  } 

  ball.y = p->y - BALL_SIZE - 0.1f;
  sound.tone(600, 50);
}


// --- Логика подсчета очков и прогресса ---

void startNextRound() {
  gameState = STATE_POINT_SCORED;
  pointDelayTimer = 60;
  sound.tone(300, 50, 450, 80);
}


void handleRallyEnd(int winner) {
  bool pointScored = false; 

  if (winner == SIDE_PLAYER) {
    if (serverTurn == SIDE_PLAYER) {
      p1.score++;
      pointScored = true;
    } else {
      serverTurn = SIDE_PLAYER;
    }
  } else { // winner == SIDE_CPU
    if (serverTurn == SIDE_CPU) {
      p2.score++;
      pointScored = true;
    } else {
      serverTurn = SIDE_CPU;
    }
  }
  
  // Проверка на конец матча (требуется 15 очков)
  if (p1.score >= 15 || p2.score >= 15) {
      //  Обновляем прогресс только в режиме ТУРА ***
      if (!isSingleMatchMode) {
          if (p1.score > p2.score) {
              matchWins++; 
              if (matchWins >= TOTAL_MATCHES_PER_BEACH && currentBeach < BEACH_MALIBU) {
                  currentBeach++;
                  matchWins = 0;
                 // saveStats(); 
              } else if (matchWins >= TOTAL_MATCHES_PER_BEACH && currentBeach == BEACH_MALIBU) {
                  //saveStats();
              }
          } else {
              matchWins = 0;
              //saveStats();
          }
      }
      gameState = STATE_GAME_OVER; 
  } 
  else {
      startNextRound(); 
  }
}

void handlePointEnd(int sideThatLost) {
  if (sideThatLost == SIDE_PLAYER) {
      handleRallyEnd(SIDE_CPU); // CPU выиграл розыгрыш
  } else {
      handleRallyEnd(SIDE_PLAYER); // Игрок выиграл розыгрыш
  }
}


void checkScoring() {
  isNewMatch = false; 
  
  if (p1.hits >= 4) {
    handlePointEnd(SIDE_PLAYER);
    return;
  }
  if (p2.hits >= 4) {
    handlePointEnd(SIDE_CPU);
    return;
  }

  if (ball.y + BALL_SIZE >= FLOOR_Y) {
    if (ball.x < NET_X) {
      handlePointEnd(SIDE_PLAYER);
    } else {
      handlePointEnd(SIDE_CPU);
    }
  }
}


// --- Отрисовка ---

void drawGame() {
  arduboy.clear();
  arduboy.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE); 

  const char* beachNamesFull[] = {"SANTA MONICA", "LONG BEACH", "PACIFIC BEACH", "MALIBU"};
  const char* beachNamesShort[] = {"S. MONICA", "L. BEACH", "P. BEACH", "MALIBU"};

  if (gameState == STATE_BEACH_SELECT) { // <--- ЭКРАН ВЫБОРА ПЛЯЖА
      arduboy.setCursor(5, 5);
      arduboy.setTextSize(2);
      arduboy.print(F("BEACH TOUR"));
      arduboy.setTextSize(1);

      arduboy.setCursor(10, 30);
      arduboy.print(F("CURRENT BEACH:"));
      
      arduboy.setCursor(15, 40);
      arduboy.setTextSize(2);
      if (currentBeach >= 0 && currentBeach < TOTAL_BEACHES) {
          arduboy.print(beachNamesShort[currentBeach]);
      } else {
          arduboy.print(F("UNKNOWN"));
      }
      arduboy.setTextSize(1);
      
      arduboy.setCursor(10, 55);
      arduboy.print(F("MATCH WINS: "));
      arduboy.print(matchWins); 
      arduboy.print(F("/4"));
      
      arduboy.setCursor(SCREEN_WIDTH - 25, 55);
      arduboy.print(F("A=GO"));
      
      // Индикатор DEBUG MODE (если активен)
      if (DEBUG_MODE) {
          arduboy.setCursor(SCREEN_WIDTH - 25, 5);
          arduboy.setTextSize(1);
          arduboy.print(F("DBG"));
          
          arduboy.setCursor(5, 55); 
          arduboy.print(F("< > CHANGE"));
      }
      
      return; 
  }

  // Рисуем сетку, игроков и мяч для титульного экрана (Демо) и игровых состояний:
  if (gameState != STATE_STATS && gameState != STATE_GAME_OVER && gameState != STATE_PAUSED && gameState != STATE_BEACH_SELECT) {
      arduboy.drawRect(NET_X, NET_Y, 2, NET_HEIGHT, WHITE); 

      int currentFrame = (frameCounter / ANIMATION_SPEED) % PLAYER_FRAME_COUNT;
      
      const uint8_t* p1FramePtr = PlayerSprite + (currentFrame * PLAYER_FRAME_SIZE);
      const uint8_t* p2FramePtr = Player1Sprite + (currentFrame * PLAYER_FRAME_SIZE);
      const uint8_t* ballFramePtr = BallSprite + (currentFrame % BALL_FRAME_COUNT * BALL_FRAME_SIZE); 


      arduboy.drawBitmap((int)p1.x, (int)p1.y, p1FramePtr, PLAYER_WIDTH, PLAYER_HEIGHT, WHITE);
      arduboy.drawBitmap((int)p2.x, (int)p2.y, p2FramePtr, PLAYER_WIDTH, PLAYER_HEIGHT, WHITE);
      arduboy.drawBitmap((int)ball.x, (int)ball.y, ballFramePtr, BALL_SIZE, BALL_SIZE, WHITE);
  }
// --- Отрисовка CPU TAUNT (Сообщения) ---
    if (tauntTimer > 0) { 
    int x = p2.x + PLAYER_WIDTH / 2 - 20; 
    int y = p2.y - 10;
    
    arduboy.setCursor(x, y);
    arduboy.setTextColor(WHITE);

    
    switch (currentTauntID) {
        case 1: arduboy.print(F("EASY!")); break;
        case 2: arduboy.print(F("LAME!")); break;
        case 3: arduboy.print(F("HA-HA!")); break;
        case 4: arduboy.print(F("NOPE")); break;
        case 5: arduboy.print(F(":(")); break;
        case 6: arduboy.print(F(":P")); break;
        case 7: arduboy.print(F("BYE!")); break;
        case 8: arduboy.print(F("FAST!")); break;
        case 9: arduboy.print(F("GIFT!")); break;
        case 10: arduboy.print(F("ROOF!")); break;
        case 11: arduboy.print(F("BOOM!")); break;
        case 12: arduboy.print(F("SEE?")); break;
        case 13: arduboy.print(F("WHIFF!")); break;
        case 14: arduboy.print(F("OUT!")); break;
        case 15: arduboy.print(F("MISS!")); break;
        case 16: arduboy.print(F("SHANK!")); break;
        case 17: arduboy.print(F("OOPS!")); break;
        case 18: arduboy.print(F("NOOB!")); break;
        case 19: arduboy.print(F("THUMBS!")); break;
        case 20: arduboy.print(F("WEAK!")); break;
        case 21: arduboy.print(F("LOOK!")); break;
        case 22: arduboy.print(F("SHORT!")); break;
        case 23: arduboy.print(F("NO WAY!")); break;
        case 24: arduboy.print(F("MORE!")); break;
        case 25: arduboy.print(F("POOP!")); break;
        case 26: arduboy.print(F("OH..NO!")); break;
        case 27: arduboy.print(F("NO TOUCH!")); break;
        case 28: arduboy.print(F("ONE-NIL!")); break;
    }
}

  if (gameState != STATE_TITLE && gameState != STATE_STATS && gameState != STATE_GAME_OVER && gameState != STATE_BEACH_SELECT) {
      const int BLOCK_SIZE = 4;
      const int PADDING = 2;
      const int INDICATOR_Y = PADDING + 6;

      // Индикаторы попаданий (Hits)
      for (int i = 0; i < p1.hits; i++) {
        int x = PADDING + (i * (BLOCK_SIZE + PADDING));
        arduboy.drawRect(x, INDICATOR_Y, BLOCK_SIZE, BLOCK_SIZE, WHITE);
      }

      for (int i = 0; i < p2.hits; i++) {
        int x = SCREEN_WIDTH - PADDING - BLOCK_SIZE - (i * (BLOCK_SIZE + PADDING));
        arduboy.drawRect(x, INDICATOR_Y, BLOCK_SIZE, BLOCK_SIZE, WHITE);
      }
      
      // Индикаторы счета
      arduboy.setCursor(45, PADDING);
      arduboy.print(p1.score);
      arduboy.setCursor(SCREEN_WIDTH - 45 - 4, PADDING);
      arduboy.print(p2.score);
      
      // Индикатор подачи
      arduboy.drawCircle(NET_X / 2, PADDING + 3, 2, WHITE); // Позиция P1
      arduboy.drawCircle(NET_X + (SCREEN_WIDTH - NET_X) / 2, PADDING + 3, 2, WHITE); // Позиция P2

      if (serverTurn == SIDE_PLAYER) {
          arduboy.fillCircle(NET_X / 2, PADDING + 3, 2, WHITE);
      } else {
          arduboy.fillCircle(NET_X + (SCREEN_WIDTH - NET_X) / 2, PADDING + 3, 2, WHITE);
      }
  }

  if (soundIndicatorTimer > 0) {
      arduboy.setCursor(SCREEN_WIDTH / 2 - 4, 2);
      arduboy.setTextSize(1);
      
      if (audioEnabled) {
          arduboy.print(F("SOUND ON"));
      } else {
          arduboy.print(F("MUTE"));
      }
  }

  if (gameState == STATE_TITLE) {
      
    // *** ИНДИКАТОР ДЕБАГ МОДА ***
    if (DEBUG_MODE) {
        arduboy.setCursor(SCREEN_WIDTH - 25, 5);
        arduboy.setTextSize(1);
        arduboy.print(F("DBG"));
    }
      
    arduboy.setCursor(5, 5);
    arduboy.setTextSize(2);
    arduboy.print(F("BVWC"));
    arduboy.setTextSize(1);
    arduboy.setCursor(60, 10);
    arduboy.setTextSize(1);
    arduboy.print(F("California"));
    
    // *** ИЗМЕНЕНИЕ: НОВОЕ МЕНЮ ***
    arduboy.setCursor(40, 30);
    arduboy.print(F("TOUR MODE"));
    arduboy.setCursor(40, 40);
    arduboy.print(F("SINGLE MATCH"));

    // Отрисовка курсора
    arduboy.setCursor(30, 30 + (titleMenuSelection * 10));
    arduboy.print(F(">"));
  }

  if (gameState == STATE_PAUSED) {
    arduboy.fillRect(20, 24, 88, 16, BLACK);
    arduboy.drawRect(20, 24, 88, 16, WHITE);
        
    arduboy.setCursor(28, 28);
    arduboy.setTextSize(2);
    arduboy.print(F("PAUSED"));
    arduboy.setTextSize(1);
    
    arduboy.setCursor(15, 48);
    arduboy.print(F("PRESS B TO RESUME"));
  }
  
  if (gameState == STATE_GAME_OVER) {
      arduboy.setTextSize(1);
      
      if (p1.score > p2.score) {
          //  Разные экраны победы ***
          if (isSingleMatchMode) {
              arduboy.setCursor(20, 20);
              arduboy.setTextSize(2);
              arduboy.print(F("YOU WIN!"));
              arduboy.setTextSize(1);
              arduboy.setCursor(10, 40);
              arduboy.print(F("PRACTICE MATCH OVER"));
          }
          else if (currentBeach == BEACH_MALIBU && matchWins == 4) {
              // Король Калифорнии!
              arduboy.setCursor(15, 10);
              arduboy.setTextSize(2);
              arduboy.print(F("CHAMPION!"));
              arduboy.setCursor(5, 30);
              arduboy.setTextSize(1);
              arduboy.print(F("YOU OWN CALIFORNIA!"));
              arduboy.setCursor(5, 40);
              arduboy.print(F("SHAMPAGNE & GIRLS!"));
          } else if (matchWins == 0 && currentBeach > BEACH_SANTA_MONICA && p1.score >= 15) {
              // Переход на следующий пляж
              arduboy.setCursor(15, 10);
              arduboy.setTextSize(2);
              arduboy.print(F("LEVEL UP!"));
              arduboy.setCursor(10, 30);
              arduboy.setTextSize(1);
              arduboy.print(F("NEXT: "));
              arduboy.print(beachNamesShort[currentBeach]);
          } else {
              // Обычная победа в матче Тура
              arduboy.setCursor(20, 20);
              arduboy.setTextSize(2);
              arduboy.print(F("YOU WIN!"));
              arduboy.setTextSize(1);
              arduboy.setCursor(15, 40);
              arduboy.print(F("ON THIS BEACH: "));
              arduboy.print(matchWins);
              arduboy.print(F("/4"));
          }
      } else {
          // Поражение (общее для обоих режимов)
          arduboy.setCursor(15, 20);
          arduboy.setTextSize(2);
          arduboy.print(F("YOU LOST!"));
          arduboy.setTextSize(1);
          if (isSingleMatchMode) {
              arduboy.setCursor(10, 40);
              arduboy.print(F("PRACTICE MATCH OVER"));
          } else {
              arduboy.setCursor(2, 40);
              arduboy.print(F("WINS RESET.START OVER"));
          }
      }
      
      arduboy.setCursor(25, 55);
      arduboy.print(F("PRESS A OR B"));
  }

  if (gameState == STATE_STATS) {
      arduboy.setCursor(35, 10);
      arduboy.setTextSize(2);
      arduboy.print(F("STATS"));
      arduboy.setTextSize(1);
      
      arduboy.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
      
      arduboy.setCursor(15, 30);
      arduboy.print(F("TOTAL WINS: "));
      arduboy.print(totalWins);
      
      arduboy.setCursor(15, 40);
      arduboy.print(F("MATCHES PLAYED: "));
      arduboy.print(totalGames);
      
      if (totalGames > 0) {
          int winRate = (totalWins * 100) / totalGames;
          arduboy.setCursor(15, 50);
          arduboy.print(F("WIN RATE: "));
          arduboy.print(winRate);
          arduboy.print(F("%"));
      } else {
          arduboy.setCursor(15, 50);
          arduboy.print(F("PLAY FIRST MATCH!"));
      }
  }
}
