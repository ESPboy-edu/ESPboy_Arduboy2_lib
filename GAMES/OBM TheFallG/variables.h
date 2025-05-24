uint8_t frameNumber;
int introCountdown = 400;
int yIntro = 0;
int xIntro = 0;

uint8_t flashTimer = 23;
bool firstRun = true; // false, skip intro
bool start = false; // true, skip menu

uint8_t anim2;
uint8_t anim3;
uint8_t anim6;

uint8_t blockW = 9;
uint8_t blockH = 19;
uint8_t blockR[21][20];
uint8_t blockL[21][20];

int screenX = 0; // increment when doing horiziontal block flips
int scrollX = 0;
int megaY = 0;
int scrollY;
bool right;
int offset = 3;

uint8_t debrisNumber = 4;
uint8_t debrisExist[5];
uint8_t debrisX[5];
uint8_t debrisY[5];
uint8_t debrisXvel[5];
uint8_t debrisYvel[5];

int acceleration;

bool pressedA;

uint8_t fuel = 100;
int frequency = 250;

int playerC = 7; // 7
int zoom = 15; // 15

const uint24_t cloud[] = {0,0,0,cloudb,0,cloudc,0,cloudd,0,cloude,0,cloudf,0,cloudg,0,cloudh};
const uint24_t coin[] = {0,0,0,0,0,coinb,0,coinc,0,coind,0,coine,0,coinf,0,coing};
const uint24_t spike[] = {0,0,0,0,0,spikeb,0,spikec,0,spiked,0,spikee,0,spikef,0,spikeg};
const uint24_t edge[] = {0,0,0,0,0,spherem,0,spheren,0,sphereo,0,spherep,0,sphereq,0,spherer};
const uint24_t baloon[] = {0,0,0,baloona,0,baloonb,0,baloonc,0,baloond,0,baloone,0,baloonf,0,baloong};

uint8_t deathTimer = 0;
uint8_t winTimer = 0;
uint8_t deathFall = 0;
int gameOverTimer = 0;
uint8_t score = 0;



