bool start;
bool dead;
bool win;

uint8_t frameNumber = 0;
int counter = 0;
uint8_t anim2;
uint8_t anim3;
uint8_t anim4;
uint8_t anim7;
uint8_t anim14;
uint8_t anim33;
uint8_t anim50;
uint8_t anim80;
uint8_t anim250;
int offsetY;

int dataRead = 0;
int eventTime = 150; //countdown untill next event
int eventType = 1; //type of event
int yCords = 60;

int goTimer = 0;

int dataPass;

uint8_t starNumber = 100;
int starX[101];
int starY[101];
uint8_t starZ[101];

int playerX = 8;
int playerY = 24;
int deathTimer;
int respawnTimer;
uint8_t lives = 3;

bool frameUp;
bool frameCentre = true;
bool frameDown;

uint8_t sigmaNumber = 9;
int sigmaX[10];
int sigmaY[10];
bool sigmaExist[10];

uint8_t plasmaNumber = 9;
int plasmaX[10];
int plasmaY[10];
int plasmaXvel[10];
int plasmaYvel[10];
bool plasmaExist[10];

uint8_t enemyNumber = 10;
int enemyX[11];
int enemyY[11];
int enemyXvel[11];
int enemyYvel[11];
uint8_t enemyType[11];
uint8_t enemyDataRead[11];
uint8_t enemyTimer[11];
uint8_t enemyExist[11];

uint8_t explodeNumber = 10;
int explodeX[11];
int explodeY[11];
uint8_t explodeExist[11];

int heartX;
int heartY;
bool heartExist;

int powerUpX;
int powerUpY;
int powerUpExist;

uint8_t powerLevel;

uint8_t laserTimer;

uint8_t bomb = 3;
uint8_t bombTimer;
int bombUpX;
int bombUpY;
int bombUpExist;

uint8_t shotNumber = 3;
int shotX[4];
int shotXvel[4];
int shotY[4];
int shotYvel[4];
bool shotExist;

int bossExist=50;
int endExist=50;
int winTimer;

int menuY = 70;
int menuSongTimer;
