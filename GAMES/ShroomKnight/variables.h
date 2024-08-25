// Global Variables

// Meta Screen Variables tiles are 160x160

int timer;
int timer10;

uint8_t roomTiles[24][24]; // max room size 32x32

uint8_t roomTileWidth; // 0 to 23
uint8_t roomTileHeight; // 0 to 23

uint8_t borX;
uint8_t borY;

int roomDimWidth;
int roomDimHeight;
int roomDimCentreX;
int roomDimCentreY;
int offsetAjustX;
int offsetAjustY;

int roomSelect = 0;
int roomRead = 0;

// Scroll Variables

//int scrollX = 1920; // 1920 center of screen location in meta space
int scrollX; // 1920 center of screen location in meta space
int scrollScreenX; // screen scale cord
int scrollXvelo;

//int scrollY = 1920; // 1920 center of screen location in meta space
int scrollY; // 1920 center of screen location in meta space
int scrollScreenY; // screen scale cord
int scrollYvelo;

int scrollLeaderX = 0;

uint8_t driftSpeedX = 25; // speed scroll drifts to new location
uint8_t driftSpeedY = 10;

// Portal Variables

//int portalX = 1280; // portal location corner of view port
int portalX; // portal location corner of view port
int screenX;

int porDestX; // portal destination /// Linked to scrollX
int portalXvelo = 0; // speed of transition

//int portalY = 1600; // portal location corner of view port
int portalY; // portal location corner of view port
int screenY;

int porDestY; // portal destination /// Linked to scrollY
int portalYvelo = 0; // speed of transition

// Player Variables

uint8_t lives = 3;

//int playerLocateX = 1840; // change when player actually moves 1920 - 80
int playerLocateX; // change when player actually moves 1920 - 80
int playerVeloX;
//int playerLocateY = 1840; // change when player actually moves 1920 - 80
int playerLocateY; // change when player actually moves 1920 - 80
int playerVeloY;


uint8_t roomNumber = 1;
int playerTileX = 4;
int playerTileY = 2;
int modX = 0;
bool swapRoom; // for going through doorway
bool roomChanged = false;

//int playerX = 640; // 640 // player location on screen meta space (needed for moving outside scroll)
int playerX = 640;
int playerScreenX;

//int playerY = 310; // 320 // player location on screen meta space (needed for moving outside scroll)
int playerY = 310;
int playerScreenY;


int offsetX = 0; // to modify location of objects in relation to a screen scroll (without the object moving relative to the screen)
int offsetY = 0;

//////

uint8_t playerDeathCountdown;
uint8_t playerWinCountdown;
uint8_t playerAttackCountdown;
uint8_t screenChangeCountdown;

uint8_t shakeScreenCountdown;
int screenPushX = 0;
int screenPushY = 0;

bool faceRight = true;
bool stoppedX = true;
bool still;
bool jumping = false;
bool falling;
bool land = false;
bool aPressed;

bool bPressed;

bool firstJump;
bool secondJump;

bool tileRight;
bool tileLeft;
bool tileBelow;
bool tileAbove;

// Power-Ups

uint8_t displayDelay = 0;
uint8_t textRef;

bool lookIconExist = false;
int lookIconX;
int lookIconY;
bool lookSkill = false;

bool doubleIconExist = false;
int doubleIconX;
int doubleIconY;
bool doubleSkill = false;

bool swordIconExist = false;
int swordIconX;
int swordIconY;
bool swordSkill = false;

bool gunIconExist = false;
int gunIconX;
int gunIconY;
bool gunSkill = false;

uint8_t arrowNumber = 3;
bool arrowExist[3];
bool arrowGoRight[3];
int arrowX[3];
int arrowY[3];
int arrowScreenX[3];
int arrowScreenY[3];

// General Variables

bool startGame = false;
uint8_t anim3 = 1;
uint8_t animSlow3 = 1;
uint8_t counter = 1;

// Enemy Variables

uint8_t enemyNumber = 3;
int enemySee;
uint8_t enemyExist[3];
bool enemyGoRight[3];
int enemyX[3];
int enemyY[3];
int enemyScreenX[3];
int enemyScreenY[3];

uint8_t droneNumber = 3;
bool droneExist[3];
bool droneGoRight[3];
int droneX[3];
int droneY[3];
int droneScreenX[3];
int droneScreenY[3];

int bossExist;
bool bossGoRight;
int bossX;
int bossY;
int bossScreenX;
int bossScreenY;

// Bits

uint8_t spawnBitCountdown;
uint8_t bitNumber = 5;
bool bitLand;
bool bitHit;
bool bitDie;
bool bitArrow;
int bitPlaceX;
int bitPlaceY;
int bitExist[6];
int bitX[6];
int bitY[6];
int bitScreenX[6];
int bitScreenY[6];

