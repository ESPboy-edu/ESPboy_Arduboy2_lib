// 1. De tekst in PROGMEM zetten
PROGMEM const char message[] = "Good morning";

// 2. De buffer in RAM
unsigned char textBox[60];

void setup() {
  // Eerst de hele buffer leegmaken (alle nullen)
  memset(textBox, 0, sizeof(textBox));

  // Tekst uit PROGMEM naar textBox kopiëren
  // (strcpy_P stopt automatisch bij de null-terminator)
  strcpy_P((char*)textBox, message);

  // Nu bevat textBox:
  // "Good morning" + de rest van de 60 bytes is 0 (leeg)
}



PROGMEM const char message[] = "Good morning";

void setup() {
  memset(textBox, 0, sizeof(textBox));               // alles leeg
  memcpy_P(textBox, message, strlen_P(message));      // alleen de karakters kopiëren
}

// met font zonder nullen op het einde

// Voorbeeld: startadres van een teken
// '0'..'9' → index 0..9
// 'A'..'Z' → index 10..35
uint16_t index = (c >= 'A') ? (c - 'A' + 10) : (c - '0');
const uint8_t* glyph = letters + index * 3;

PROGMEM const char message[] = "Good morning";

unsigned char textBox[60];

void setup() {
  memset(textBox, 0, sizeof(textBox));   // alles eerst leeg

  uint8_t i = 0;                          // positie in textBox
  uint8_t len = strlen_P(message);

  for (uint8_t j = 0; j < len; j++) {
    char c = pgm_read_byte(message + j);

    // Spatie of niet-ondersteund teken overslaan of als 0 behandelen
    if (c == ' ') {
      textBox[i++] = 0;                   // of gewoon een 0 als separator
      continue;
    }

    // Alleen A-Z en 0-9 accepteren (naar uppercase)
    if (c >= 'a' && c <= 'z') c -= 32;

    if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z')) {
      textBox[i++] = c;                   // het karakter zelf
      textBox[i++] = 0;                   // altijd een 0 erna
    }
  }

  // Rest van textBox blijft 0 (al gedaan met memset)
}

// alles altijd in hoofdletters en meerdere boodschappen

// Alle boodschappen achter elkaar in één PROGMEM-array
// Elke boodschap eindigt met \0
PROGMEM const char messages[] =
  "GOOD MORNING\0"
  "HELLO ARDUBOY\0"
  "HAVE A NICE DAY\0"
  "GAME OVER\0";

// Haalt de boodschap met index 'index' op en zet hem in textBox
// (met een 0 tussen elk karakter, zoals je eerder wilde)
void loadMessage(uint8_t index) {
  memset(textBox, 0, sizeof(textBox));

  // Zoek het begin van de gevraagde boodschap
  const char* p = messages;
  while (index > 0) {
    // Sla de huidige string over (tot en met de \0)
    while (pgm_read_byte(p) != 0) p++;
    p++;          // spring over de \0 heen
    index--;
  }

  // Nu p wijst naar het begin van de gewenste boodschap
  uint8_t i = 0;
  char c;
  while ((c = pgm_read_byte(p++)) != 0 && i < 58) {
    textBox[i++] = c;     // karakter
    textBox[i++] = 0;     // separator 0
  }
}

loadMessage(0);   // → "GOOD MORNING"
loadMessage(1);   // → "HELLO ARDUBOY"
loadMessage(2);   // → "HAVE A NICE DAY"

//////////////////////////////////////////
met de mogelijkheid om nummers toe te voegen


// -------------------------------------------------
// PROGMEM boodschappen
// -------------------------------------------------
PROGMEM const char messages[] =
  "GOOD MORNING\0"
  "HELLO ARDUBOY\0"
  "HAVE A NICE DAY\0"
  "GAME OVER\0"
  "SCORE\0"
  "HIGH SCORE\0"
  "LEVEL\0";

// -------------------------------------------------
// textBox
// -------------------------------------------------
unsigned char textBox[60];

// -------------------------------------------------
// loadMessage(whatMessage, numbersToShow, showNumbers)
// -------------------------------------------------
// showNumbers = true  → toon het getal (ook als het 0 is)
// showNumbers = false → toon alleen de tekst
// -------------------------------------------------
void loadMessage(uint8_t whatMessage, unsigned int numbersToShow = 0, bool showNumbers = false) {
  memset(textBox, 0, sizeof(textBox));

  uint8_t i = 0;

  // ---------- 1. Vaste boodschap ophalen ----------
  const char* p = messages;
  uint8_t count = whatMessage;

  while (count > 0) {
    while (pgm_read_byte(p) != 0) p++;
    p++;
    count--;
  }

  // Tekst kopiëren + 0-separator
  char c;
  while ((c = pgm_read_byte(p++)) != 0 && i < 48) {
    textBox[i++] = c;
    textBox[i++] = 0;
  }

  // ---------- 2. Getal toevoegen (alleen als showNumbers == true) ----------
  if (showNumbers) {
    // Extra 0 als scheiding tussen tekst en getal
    if (i < 58) textBox[i++] = 0;

    // Getal omzetten naar cijfers (max 5 cijfers)
    char digits[6];
    uint8_t len = 0;
    unsigned int n = numbersToShow;

    if (n == 0) {
      digits[len++] = '0';
    } else {
      while (n > 0 && len < 5) {
        digits[len++] = '0' + (n % 10);
        n /= 10;
      }
    }

    // Cijfers in de juiste volgorde schrijven
    while (len > 0 && i < 58) {
      textBox[i++] = digits[--len];
      textBox[i++] = 0;
    }
  }
}
