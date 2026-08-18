// check "ArduboyFX.h" for
//
//#define USE_LITTLEFS
// if defined then Little_FS will be used as FX data source overwise PROGMEM array fxdta[] will be used 
//
//#define USE_RLE_COMPRESSION
//if defined, then put RLE compressed data to fxdta[]

#include "ArduboyFX.h"
#include "font5x7local.h"

#ifdef USE_RLE_COMPRESSION
  extern uint8_t fxdta[];
#endif

// [STEP 1]: One-time allocated sprite cache buffer
static uint8_t* bmpCache = nullptr;
#define BMP_CACHE_SIZE 1024

#ifdef USE_LITTLEFS
  #define CACHE_BUF_SIZE 4096
  File fle;
#else
  volatile uint32_t globalAddress = 0;
  extern uint8_t fxdta[];
#endif

volatile uint32_t globalAddressSave = EEPROMWRITEOFFSET;

uint16_t FX::programDataPage = 0; // program read only data location in flash memory
uint16_t FX::programSavePage = 0; // program read and write data location in flash memory
Font     FX::font;
Cursor   FX::cursor = {0,0,0,WIDTH};

FrameControl FX::frameControl;

#ifdef USE_RLE_COMPRESSION
void FX::Rle_Decode(unsigned char *inbuf, uint32_t inSize){
    uint8_t *outBuf;
    uint16_t cntOutBuf = 0;
    uint8_t *src = inbuf;
    uint8_t dtaReaded;
    uint8_t sign;
    uint16_t i;
    
    outBuf = (uint8_t *)malloc(CACHE_BUF_SIZE);
    
    drawStringOld(12, 20, 1, "Preparing FX data");
    while (src < (inbuf + inSize)) {
        sign = pgm_read_byte(src++);
        int count = sign & 0x7F;
        if ((cntOutBuf + count) > CACHE_BUF_SIZE) { 
            fle.write(outBuf, cntOutBuf);
            cntOutBuf = 0;
            char bfrchr[5];
            itoa(100.0 / (float)inSize * (src - inbuf), bfrchr, 10);
            uint8_t lneStr = strlen(bfrchr);
            bfrchr[lneStr] = '%';
            bfrchr[lneStr + 1] = 0;
            drawStringOld(55, 40, 1, (const char *)bfrchr);
            display();
        }
        if ((sign & 0x80) == 0x80) {          
            for (i = 0; i < count; i++) {
                dtaReaded = pgm_read_byte(src);
                outBuf[cntOutBuf++] = dtaReaded;
            }
            src++;
        } else {
            for (i = 0; i < count; i++) {
                dtaReaded = pgm_read_byte(src++);
                outBuf[cntOutBuf++] = dtaReaded;
            }
        }           
    }
    delete(outBuf);
}
#endif

uint8_t FX::writeByte(uint8_t data){
  uint8_t result;
  #ifdef USE_LITTLEFS
     result = data; 
     fle.write(data);
  #else
    result = 0;
    globalAddress++;
  #endif
  return result;
}

uint8_t FX::readByte(){
 #ifdef USE_LITTLEFS
  return(fle.read());
 #else
  return pgm_read_byte(&fxdta[globalAddress++]);
 #endif
}

void FX::begin(){ 
    // [STEP 1]: Allocate sprite cache RAM buffer only when FX starts
    if (!bmpCache) {
        bmpCache = (uint8_t*)malloc(BMP_CACHE_SIZE);
    }

    EEPROM.begin(4096);

#ifdef DEBUG_INFO_ON
    Serial.begin(115200);
    Serial.println();
#endif

#ifdef USE_LITTLEFS    
    LittleFSConfig cfg;
    cfg.setAutoFormat(true);
    LittleFS.setConfig(cfg);
    LittleFS.begin();
    fle = LittleFS.open("/fxdta.bin", "r+");
    fle.seek(0, SeekEnd);
#ifdef DEBUG_INFO_ON
    Serial.println();
    Serial.println(fle.position());
#endif    
    
#ifdef USE_RLE_COMPRESSION
    if (!fle || fle.position() != UNPACKED_FILE_SIZE) {
#else
    if (!fle) {
#endif

#ifdef DEBUG_INFO_ON
      Serial.println("fxdta.bin not found!");
#endif

      fle.close();
      LittleFS.format();
      fle = LittleFS.open("/fxdta.bin", "w+");
#ifdef USE_RLE_COMPRESSION
  #ifdef DEBUG_INFO_ON
      Serial.println("Decoding file from PROGMEM...");
  #endif    
      Rle_Decode((unsigned char *)fxdta, RLE_FILE_SIZE);
#endif
      fle.close();
      fle = LittleFS.open("/fxdta.bin", "r+");
    }
    fle.seek(0, SeekSet);
#ifdef DEBUG_INFO_ON
     Serial.println("FX data OK");
#endif
#endif
}

void FX::begin(uint16_t developmentDataPage){
  programDataPage = developmentDataPage;
  begin();
}

void FX::begin(uint16_t developmentDataPage, uint16_t developmentSavePage){
  programDataPage = developmentDataPage;
  programSavePage = developmentSavePage;
  begin();
}

void FX::readJedecID(JedecID & id){
  id.manufacturer = 1;
  id.device = 2;
  id.size = 255;
}

void FX::readJedecID(JedecID* id){
  id -> manufacturer = 1;
  id -> device = 2;
  id -> size = 255;
}

bool FX::detect(){return true;}

void FX::noFXReboot(){ESP.restart();}

void FX::seekData(uint24_t address){
 #ifdef USE_LITTLEFS 
  fle.seek(address, SeekSet);
 #else
  globalAddress = address;
 #endif
}

void FX::seekDataArray(uint24_t address, uint8_t index, uint8_t offset, uint8_t elementSize){
  address += elementSize ? index * elementSize + offset : index * 256 + offset;
  seekData(address);
}

void FX::seekSave(uint24_t address){   
  globalAddressSave = address + EEPROMWRITEOFFSET;
}

uint8_t FX::readPendingUInt8(){
   return readByte();
}

uint8_t FX::readPendingLastUInt8(){
   return readByte();
}

// [STEP 3]: Batch reading multi-byte values in a single readBytes() call
uint16_t FX::readPendingUInt16(){
  uint8_t b[2];
  readBytes(b, 2);
  return ((uint16_t)b[0] << 8) | b[1];
}

uint16_t FX::readPendingLastUInt16(){
   return readPendingUInt16();
}

uint24_t FX::readPendingUInt24(){
  uint8_t b[3];
  readBytes(b, 3);
  return ((uint24_t)b[0] << 16) | ((uint16_t)b[1] << 8) | b[2];
}

uint24_t FX::readPendingLastUInt24(){
   return readPendingUInt24();
}

uint32_t FX::readPendingUInt32(){
  uint8_t b[4];
  readBytes(b, 4);
  return ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) | ((uint16_t)b[2] << 8) | b[3];
}

uint32_t FX::readPendingLastUInt32(){
   return readPendingUInt32();
}

void FX::readBytes(uint8_t* buffer, size_t length){
  #ifdef USE_LITTLEFS
    fle.readBytes((char *)buffer, length);
  #else
    memcpy_P(buffer, &fxdta[globalAddress], length);
    globalAddress += length;
  #endif
}

void FX::readBytesSave(uint8_t* buffer, size_t length){
  noInterrupts();
  for (uint16_t i = 0; i < length; i++) {
    if (globalAddressSave < 4092 - EEPROMWRITEOFFSET) {
      buffer[i] = EEPROM.read(globalAddressSave);
#ifdef DEBUG_INFO_ON
      Serial.print(buffer[i], HEX); Serial.print(" ");
#endif
    }
    globalAddressSave++;
  }
#ifdef DEBUG_INFO_ON
  Serial.println();
#endif   
  interrupts();
}

void FX::readBytesEnd(uint8_t* buffer, size_t length){
  readBytes(buffer, length);
}

uint8_t FX::readEnd(){
  return readByte(); 
}

void FX::readDataBytes(uint24_t address, uint8_t* buffer, size_t length){
  seekData(address);
  readBytes(buffer, length);
}

void FX::readSaveBytes(uint24_t address, uint8_t* buffer, size_t length){
  seekSave(address);
  readBytesSave(buffer, length);
}

uint8_t FX::loadGameState(uint8_t* gameState, size_t size){
  noInterrupts();
#ifdef DEBUG_INFO_ON
  Serial.println("Load game state");
  Serial.print("size "); Serial.println(size); 
#endif 
  seekSave(0);
  if (EEPROM.read(globalAddressSave++) == (uint8_t)(size & 255) && 
      EEPROM.read(globalAddressSave++) == (uint8_t)((size >> 8) & 255)) {
    readBytesSave(gameState, size);
    interrupts();
    return 1;
  } else {
    interrupts(); 
    return 0;
  }
}

void FX::saveGameState(const uint8_t* gameState, size_t size){ 
  noInterrupts();
#ifdef DEBUG_INFO_ON
  Serial.println("Save game state");
  Serial.print("size "); Serial.println(size); 
#endif        
  seekSave(0);    
  EEPROM.write(globalAddressSave++, (uint8_t)(size & 255));
  EEPROM.write(globalAddressSave++, (uint8_t)((size >> 8) & 255));
  for (uint16_t i = 0; i < size; i++) {
    EEPROM.write(globalAddressSave++, gameState[i]);
#ifdef DEBUG_INFO_ON
    Serial.print(gameState[i], HEX); Serial.print(" ");  
#endif
  }
#ifdef DEBUG_INFO_ON
  Serial.println();
#endif
  EEPROM.commit();
  interrupts();
}

void FX::eraseSaveBlock(uint16_t page){
  noInterrupts(); 
  seekSave(0);                 
  for (uint16_t i = 0; i < 4096 - EEPROMWRITEOFFSET; i++) {
    EEPROM.write(globalAddressSave, 0);
    globalAddressSave++;
  }
  EEPROM.commit();
  interrupts();
}

void FX::writeSavePage(uint16_t page, uint8_t* buffer){
  noInterrupts(); 
  seekSave(0);                                     
  for (uint16_t i = 0; i < 256; i++) {
    EEPROM.write(globalAddressSave, buffer[i]);
    globalAddressSave++;
  }
  EEPROM.commit();
  interrupts(); 
}

void FX::drawBitmap(int16_t x, int16_t y, uint24_t address, uint8_t frame, uint8_t mode){
  seekData(address);
  int16_t width  = readPendingUInt16();
  int16_t height = readPendingLastUInt16();
  
  if (x + width <= 0 || x >= WIDTH || y + height <= 0 || y >= HEIGHT) return;

  int16_t skipleft = 0;
  uint8_t renderwidth;
  if (x < 0) {
    skipleft = -x;
    if (width - skipleft < WIDTH) renderwidth = width - skipleft;
    else renderwidth = WIDTH;
  } else {
    if (x + width > WIDTH) renderwidth = WIDTH - x;
    else renderwidth = width;
  }

  int16_t skiptop;     
  int8_t renderheight; 
  if (y < 0) {
    skiptop = -y & -8; 
    if (height - skiptop <= HEIGHT) renderheight = height - skiptop;
    else renderheight = HEIGHT + (-y & 7);
    skiptop = fastDiv8(skiptop); 
  } else {
    skiptop = 0;
    if (y + height > HEIGHT) renderheight = HEIGHT - y;
    else renderheight = height;
  }
  
  uint24_t offset = (uint24_t)(frame * ((height + 7) / 8) + skiptop) * width + skipleft;
  if (mode & dbmMasked) {
    offset += offset; 
    width += width;
  }
  address += offset + 4; 
  int8_t displayrow = (y >> 3) + skiptop;
  int16_t displayoffset = displayrow * WIDTH + x + skipleft;
  uint8_t yshift = bitShiftLeftUInt8(y); 
  uint8_t lastmask = bitShiftRightMaskUInt8(8 - height); 
  
  seekData(address);
  int32_t initAddress = address;
  uint8_t *bitmapBuffer = nullptr;
  int32_t pointertoBmp = 0;

  // [STEP 4]: Optimize buffer size calculation. Read only active rendered rows instead of full sprite height
  size_t numRenderRows = (renderheight + 7) / 8;
  size_t bmpSize = (size_t)width * numRenderRows;
  bool allocatedLocally = false;

  if (bmpCache && bmpSize <= BMP_CACHE_SIZE) {
    bitmapBuffer = bmpCache;
  } else {
    bitmapBuffer = (uint8_t *) malloc (bmpSize);
    allocatedLocally = true;
  }

  if (bitmapBuffer) {
    readBytes(bitmapBuffer, bmpSize);
  }
  
  do { 
    pointertoBmp = address - initAddress;
    address += width;
    mode &= ~((1 << dbfExtraRow));
    if (yshift != 1 && displayrow < (HEIGHT / 8 - 1)) mode |= (1 << dbfExtraRow);
    uint8_t rowmask = 0xFF;
    if (renderheight < 8) rowmask = lastmask;
    for (uint8_t c = 0; c < renderwidth; c++) {
      // [STEP 4]: Direct RAM read when bitmapBuffer is present
      uint8_t bitmapbyte = bitmapBuffer ? bitmapBuffer[pointertoBmp++] : readUnsafe();
           
      if (mode & (1 << dbfReverseBlack)) bitmapbyte ^= rowmask;
      uint8_t maskbyte = rowmask;
      if (mode & (1 << dbfWhiteBlack)) maskbyte = bitmapbyte;
      if (mode & (1 << dbfBlack)) bitmapbyte = 0;
      uint16_t bitmap = multiplyUInt8(bitmapbyte, yshift);
      if (mode & (1 << dbfMasked)) {
        uint8_t tmp = bitmapBuffer ? bitmapBuffer[pointertoBmp++] : readUnsafe();
        if ((mode & dbfWhiteBlack) == 0) maskbyte = tmp;
      }
      uint16_t mask = multiplyUInt8(maskbyte, yshift);
      if (displayrow >= 0) {
        uint8_t pixels = bitmap;
        uint8_t display = Arduboy2Base::sBuffer[displayoffset];
        if ((mode & (1 << dbfInvert)) == 0) pixels ^= display;
        pixels &= mask;
        pixels ^= display;
        Arduboy2Base::sBuffer[displayoffset] = pixels;
      }
      if (mode & (1 << dbfExtraRow)) {
        uint8_t display = Arduboy2Base::sBuffer[displayoffset + WIDTH];
        uint8_t pixels = bitmap >> 8;
        if ((mode & dbfInvert) == 0) pixels ^= display;
        pixels &= mask >> 8;
        pixels ^= display;
        Arduboy2Base::sBuffer[displayoffset + WIDTH] = pixels;
      }
      (mode & (1 << dbfFlip)) ? displayoffset-- : displayoffset++;
    }
    displayoffset += WIDTH;
    (mode & (1 << dbfFlip)) ?  displayoffset += renderwidth : displayoffset -= renderwidth;
    displayrow ++;
    renderheight -= 8;
    
  } while (renderheight > 0);
  
  if (allocatedLocally && bitmapBuffer) {
    free(bitmapBuffer);
  }
}

void FX::setFrame(uint24_t frame, uint8_t repeat){
  frameControl.start   = frame;
  frameControl.current = frame;
  frameControl.repeat  = repeat;
  frameControl.count   = repeat;
}

uint8_t FX::drawFrame(){
  uint24_t frame = drawFrame(frameControl.current);
  uint8_t moreFrames;
  moreFrames = (frame != 0) | frameControl.count;
  if (frameControl.count > 0) {
    frameControl.count--;
  } else {
    frameControl.count = frameControl.repeat;
    if (!moreFrames) frame = frameControl.start;
    frameControl.current = frame;
  }
  return moreFrames;
}

// [STEP 2]: Batch read frame header (9 bytes at once)
uint24_t FX::drawFrame(uint24_t address){
  for(;;) {
    seekData(address);
    address += 9;
    uint8_t buf[9];
    readBytes(buf, 9);
    
    int16_t x = (int16_t)((buf[0] << 8) | buf[1]);
    int16_t y = (int16_t)((buf[2] << 8) | buf[3]);
    uint24_t bmp = ((uint24_t)buf[4] << 16) | ((uint16_t)buf[5] << 8) | buf[6];
    uint8_t frame = buf[7];
    uint8_t mode = buf[8];

    drawBitmap(x, y, bmp, frame, mode);
    if (mode & dbmEndFrame) return address;
    if (mode & dbmLastFrame) return 0;
  }
}

void FX::readDataArray(uint24_t address, uint8_t index, uint8_t offset, uint8_t elementSize, uint8_t* buffer, size_t length){
  seekDataArray(address, index, offset, elementSize);
  readBytesEnd(buffer, length);
}

uint8_t FX::readIndexedUInt8(uint24_t address, uint8_t index){
  seekDataArray(address, index, 0, sizeof(uint8_t));
  return readByte();
}

uint16_t FX::readIndexedUInt16(uint24_t address, uint8_t index){
  seekDataArray(address, index, 0, sizeof(uint16_t));
  return readPendingLastUInt16();
}

uint24_t FX::readIndexedUInt24(uint24_t address, uint8_t index){
  seekDataArray(address, index, 0, sizeof_uint24_t);
  return readPendingLastUInt24();
}

uint32_t FX::readIndexedUInt32(uint24_t address, uint8_t index){
  seekDataArray(address, index, 0, sizeof_uint24_t);
  return readPendingLastUInt32();
}

void FX::displayPrefetch(uint24_t address, uint8_t* target, uint16_t len, bool clear){
  seekData(address);
  readBytes(target, len);
  display(clear);
}

void FX::display(){
  Arduboy2Base::display();
}

void FX::display(bool clear){
  Arduboy2Base::display(clear);
}

void FX::setFont(uint24_t address, uint8_t mode){
  font.address = address;
  font.mode = mode;
  seekData(address);
  font.width = readPendingUInt16();
  font.height = readPendingLastUInt16();
}

void FX::setFontMode(uint8_t mode){
  font.mode = mode;
}

void FX::setCursor(int16_t x, int16_t y){
  cursor.x = x;
  cursor.y = y;
}

void FX::setCursorX(int16_t x){
  cursor.x = x;
}

void FX::setCursorY(int16_t y){
  cursor.y = y;
}

void FX::setCursorRange(int16_t left, int16_t wrap){
  cursor.left = left;
  cursor.wrap = wrap;
}

void FX::setCursorLeft(int16_t left){
  cursor.left = left;
}

void FX::setCursorWrap(int16_t wrap){
  cursor.wrap = wrap;
}

void FX::drawChar(uint8_t c){
  if (c == '\r') return;
  uint8_t mode = font.mode;
  int16_t x = cursor.x;
  int16_t y = cursor.y;
  if (c != '\n') {
    drawBitmap(x, y, font.address, c, mode);
    if (mode & dcmProportional) {
      seekData(font.address - 256 + c);
      x += readByte();
    } else {
      x += font.width;
    }
  }
  if ((c == '\n') || (x >= cursor.wrap)) {
    x = cursor.left;
    y += font.height;
  }
  setCursor(x,y);
}

void FX::drawString(const uint8_t* buffer){
  for(;;) {
    uint8_t c = *buffer++;
    if (c) drawChar(c);
    else break;
  }
}

void FX::drawString(const char* str){
  FX::drawString((const uint8_t*)str);
}

// [STEP 5]: Buffered string reading from FX storage in a single batch call
void FX::drawString(uint24_t address){
  uint8_t strBuf[64];
  bool finished = false;

  while (!finished) {
    seekData(address);
    readBytes(strBuf, sizeof(strBuf));
    
    for (size_t i = 0; i < sizeof(strBuf); i++) {
      uint8_t c = strBuf[i];
      address++;
      if (c == 0) {
        finished = true;
        break;
      }
      drawChar(c);
    }
  }
}

void FX::drawNumber(int16_t n, int8_t digits){
  drawNumber((int32_t)n, digits);
}

void FX::drawNumber(uint16_t n, int8_t digits){
  drawNumber((uint32_t)n, digits);
}

void FX::drawNumber(int32_t n, int8_t digits){
  if (n < 0) {
    n = -n;
    drawChar('-');
  } else if (digits != 0) {
    drawChar(' ');
  }
  drawNumber((uint32_t)n, digits);
}

void FX::drawNumber(uint32_t n, int8_t digits){
  uint8_t buf[33]; 
  uint8_t *str = &buf[sizeof(buf) - 1];
  *str = '\0';
  do {
    char c = n % 10;
    n /= 10;
    *--str = c + '0';
    if ((digits > 0) && (--digits == 0)) break;
    if ((digits < 0) && (++digits == 0)) break;
  } while(n);
  while (digits > 0) {--digits; *--str = '0';}
  while (digits < 0) {++digits; *--str = ' ';}
  drawString(str);
}

void FX::drawStringOld(int16_t x, int16_t y, uint8_t color, const char* buffer){
  for(;;) {
    uint8_t c = *buffer++;
    if (c) drawCharOld(x, y, c, color, !color, 1);
    else break;
    x += 6;
  }
}

void FX::drawPixelOld(int16_t x, int16_t y, uint8_t color){
  if (x < 0 || x > (WIDTH-1) || y < 0 || y > (HEIGHT-1)) return;
  uint8_t row = (uint8_t)y / 8;
  if (color) {
    Arduboy2Base::sBuffer[(row*WIDTH) + (uint8_t)x] |= _BV((uint8_t)y % 8);
  } else {
    Arduboy2Base::sBuffer[(row*WIDTH) + (uint8_t)x] &= ~_BV((uint8_t)y % 8);
  }
};

void FX::drawCharOld(int16_t x, int16_t y, uint8_t c, uint8_t color, uint8_t bg, uint8_t size){
  bool drawBackground = bg != color;
  
  uint8_t characterWidth = 5;
  uint8_t characterHeight = 8;
  uint8_t characterSpacing = 1;
  uint8_t lineSpacing = 0;
  uint8_t fullCharacterWidth = characterWidth + characterSpacing;
  uint8_t fullCharacterHeight = characterHeight + lineSpacing;
  
  const uint8_t* bitmap = &font5x7local[c * characterWidth * ((characterHeight + 8 - 1) / 8)];
  for (uint8_t i = 0; i < fullCharacterWidth; i++) {
    uint8_t column;
    if (characterHeight <= 8) {
      column = (i < characterWidth) ? pgm_read_byte(bitmap++) : 0;
    } else {
      column = 0;
    }

    for (uint8_t j = 0; j < characterHeight; j++) {
      if (characterHeight > 8) {
        if ((j % 8 == 0) && (i < characterWidth)) {
          column = pgm_read_byte(bitmap++);
        }
      }
      uint8_t pixelIsSet = column & 0x01;
      if (pixelIsSet || drawBackground) {
        for (uint8_t a = 0; a < size; a++) {
          for (uint8_t b = 0; b < size; b++) {
            drawPixelOld(x + (i * size) + a, y + (j * size) + b, pixelIsSet ? color : bg);
          }
        }
      }
      column >>= 1;
    }

    if (drawBackground) {
      for (uint8_t j = characterHeight; j < fullCharacterHeight; j++) {
        for (uint8_t a = 0; a < size; a++) {
          for (uint8_t b = 0; b < size; b++) {
            drawPixelOld(x + (i * size) + a, y + (j * size) + b, bg);
          }
        }
      }
    }
  }
}