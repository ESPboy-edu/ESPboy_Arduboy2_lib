int frameNumber;
int introCountdown = 400;
int yIntro = 0;
int xIntro = 0;
int flashTimer = 23;
bool firstRun=true; // skip intro
bool start; // skip menu

int anim2;
int anim3;
int anim4;
int anim25;
int anim50;

int xOffset;
int yOffset;

int boardXY[6][6];
int boardNextXY[6][6];

int dataRead = 0;
int boardObject;
int levelRead = 0;
int cycleLength;

int xStep;
int yStep;

int playerX = 2; // 0,1,2,3,4,
int playerY = 2;

uint8_t flipCountdown;
uint8_t levelQuater;
int moveCountdown;
int levelCountdown = 1000;
int cycleCountdown = 100;
int level;
int blockCount;

bool buttonA;
//bool buttonB;

int playerScopeY;
int yLocation;
int yDestination;
int stomp;
int deathTimer = 0;
int gameOverTimer = 0;
int score;


int rainNumber = 19;
uint8_t rainExist[20]; // rain lifespan
int rainX[20];
int rainY[20];
int xPass;
int yPass;
int xChangePos;
int yChangePos;
int changeDelay;

