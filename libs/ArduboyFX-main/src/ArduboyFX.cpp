
// check "ArduboyFX.h" for #define USE_LITTLEFS
// if defined then Little_FS will be used as FX data source overwise PROGMEM array fxdta[] will be used 


#include "ArduboyFX.h"

//font5x7
#include "font5x7local.h"



#ifdef USE_LITTLEFS
  #define RLE_FILE_SIZE 382127
  #define UNPACKED_FILE_SIZE 956419

  #define CACHE_BUF_SIZE 4098
  File fle;
  extern uint8_t fxdta[];
#else
  volatile uint32_t globalAddress=0;
  extern uint8_t fxdta[];
#endif

uint16_t FX::programDataPage = 0; // program read only data location in flash memory
uint16_t FX::programSavePage = 0; // program read and write data location in flash memory
Font     FX::font;
Cursor   FX::cursor = {0,0,0,WIDTH};

FrameControl FX::frameControl;


void FX::Rle_Decode(unsigned char *inbuf, uint32_t inSize){
    uint8_t *outBuf;
    uint16_t cntOutBuf=0;
    uint8_t *src=inbuf;
    uint8_t dtaReaded;
    uint8_t sign;
    uint16_t i;
    uint16_t decSize=0;
    uint16_t count=0;
    
    outBuf = (uint8_t *)malloc(CACHE_BUF_SIZE);
    
    drawStringOld(12, 20, 1,"Preparing FX data");
    while(src<(inbuf+inSize)){
        sign=pgm_read_byte(src++);
        int count=sign & 0x7F;
        if((cntOutBuf+count)>CACHE_BUF_SIZE){ 
            fle.write(outBuf, cntOutBuf);
            cntOutBuf=0;
            char bfrchr[5];
            itoa(100.0/(float)inSize * (src-inbuf), bfrchr, 10);
            uint8_t lneStr = strlen(bfrchr);
            bfrchr[lneStr]='%';
            bfrchr[lneStr+1]=0;
            drawStringOld(55, 40, 1,(const char *)bfrchr);
            display();
        }
        if((sign&0x80)==0x80){          
            for(i=0;i<count;i++){
                dtaReaded = pgm_read_byte(src);
                outBuf[cntOutBuf++]=dtaReaded;
            }
            src++;
        }else{
            for(i=0;i<count;i++){
                dtaReaded = pgm_read_byte(src++);
                outBuf[cntOutBuf++]=dtaReaded;
            }
        }           
    }
    delete(outBuf);
}






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



static uint8_t  result;

uint8_t IRAM_ATTR FX::readByte(){
 #ifdef USE_LITTLEFS
  result = fle.read();
 #else
  result = pgm_read_byte(&fxdta[globalAddress]);
  globalAddress++;
 #endif
  return result;
}




void FX::begin(){ 
  #ifdef USE_LITTLEFS
    //Serial.println();
    LittleFSConfig cfg;
    cfg.setAutoFormat(true);
    LittleFS.setConfig(cfg);
    LittleFS.begin();
    fle = LittleFS.open("/fxdta.bin", "r+");
    fle.seek(0, SeekEnd);
    //Serial.println();
    //Serial.println(fle.position());
    if (!fle || fle.position()!=UNPACKED_FILE_SIZE) {
      //Serial.println("fxdta.bin not found!");
      fle.close();
      LittleFS.format();
      fle = LittleFS.open("/fxdta.bin", "w+");
      //Serial.println("Decoding file from PROGMEM...");
      Rle_Decode((unsigned char *)fxdta, RLE_FILE_SIZE);
      fle.close();
      fle = LittleFS.open("/fxdta.bin", "r+");
    }
    fle.seek(0,SeekSet);
     //Serial.println("FX data OK");
  #endif
}


void FX::begin(uint16_t developmentDataPage){
  begin();
}


void FX::begin(uint16_t developmentDataPage, uint16_t developmentSavePage){
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
  seekData(address);
}


uint8_t FX::readPendingUInt8(){
   return readByte();
}


uint8_t FX::readPendingLastUInt8(){
   return readByte();
}


uint16_t FX::readPendingUInt16(){
   return (((uint16_t)readByte() << 8) | readByte());
 }


uint16_t FX::readPendingLastUInt16(){
   return readPendingUInt16();
 }


uint24_t FX::readPendingUInt24(){
   return (((uint24_t)readPendingUInt16() << 8) | readByte());
}


uint24_t FX::readPendingLastUInt24(){
   return readPendingUInt24();
}


uint32_t FX::readPendingUInt32(){
   return (((uint32_t)readPendingUInt16() << 16) | readPendingUInt16());
}


uint32_t FX::readPendingLastUInt32(){
   return readPendingUInt32();
}



void FX::readBytes(uint8_t* buffer, size_t length){
  #ifdef USE_LITTLEFS
    fle.readBytes((char *)buffer, length);
  #else
    for (size_t i = 0; i < length; i++)
      buffer[i] = readByte();
  #endif
}


void FX::readBytesEnd(uint8_t* buffer, size_t length){
  readBytes(buffer, length);
}


uint8_t FX::readEnd(){
    return readByte(); // read last byte and disable flash
}


void FX::readDataBytes(uint24_t address, uint8_t* buffer, size_t length){
  seekData(address);
  readBytes(buffer, length);
}


void FX::readSaveBytes(uint24_t address, uint8_t* buffer, size_t length){
  seekSave(address);
  readBytes(buffer, length);
}

uint8_t FX::loadGameState(uint8_t* gameState, size_t size) // ~54 bytes
{
  seekSave(0);
  uint8_t result = 0;
  while (readPendingUInt16() == size) // if gameState size not equal, most recent gameState has been read or there is no gameState
  {
    for (uint16_t i = 0; i < size; i++)
    {
      uint8_t data = readPendingUInt8();
      gameState[i] = data;
    }
    {
      result = 1; // signal gameState loaded
    }
  }
  readByte();
  return result;
}

void FX::saveGameState(const uint8_t* gameState, size_t size) // ~152 bytes locates free space in 4K save block and saves the GamesState.
{                                                       //            if there is not enough free space, the block is erased prior to saving
  uint16_t addr = 0;
  for(;;) // locate end of previous gameStates
  {
    seekSave(addr);
    if (readPendingLastUInt16() != size) break; //found end of previous gameStates
    addr += size + 2;
  }
  if ((addr + size) > 4094) //is there enough space left? last two bytes of 4K block must always be unused (0xFF)
  {
    eraseSaveBlock(0); // erase save block
    addr = 0;          // write saveState at the start of block
  }

  while (size)
  {
    seekData((uint24_t)(programSavePage << 8) + addr);
    do
    {
      writeByte(*gameState++);
      if (--size == 0) break;
    }
    while ((uint8_t)++addr); // write bytes until end of a page
  }
}


void  FX::eraseSaveBlock(uint16_t page){
  seekData((uint24_t)(programSavePage + page) << 8);
}


void FX::writeSavePage(uint16_t page, uint8_t* buffer){
  seekData((uint24_t)(programSavePage + page) << 8);
  uint8_t i = 0;
  do
  {
    writeByte(buffer[i]);
  }
  while (i++ < 255);
}

void FX::drawBitmap(int16_t x, int16_t y, uint24_t address, uint8_t frame, uint8_t mode){
  // read bitmap dimensions from flash

  seekData(address);
  int16_t width  = readPendingUInt16();
  int16_t height = readPendingLastUInt16();
  
  // return if the bitmap is completely off screen
  if (x + width <= 0 || x >= WIDTH || y + height <= 0 || y >= HEIGHT) return;

  // determine render width
  int16_t skipleft = 0;
  uint8_t renderwidth;
  if (x<0)
  {
    skipleft = -x;
    if (width - skipleft < WIDTH) renderwidth = width - skipleft;
    else renderwidth = WIDTH;
  }
  else
  {
    if (x + width > WIDTH) renderwidth = WIDTH - x;
    else renderwidth = width;
  }

  //determine render height
  int16_t skiptop;     // pixel to be skipped at the top
  int8_t renderheight; // in pixels
  if (y < 0)
  {
    skiptop = -y & -8; // optimized -y / 8 * 8
    if (height - skiptop <= HEIGHT) renderheight = height - skiptop;
    else renderheight = HEIGHT + (-y & 7);
    skiptop = fastDiv8(skiptop); // pixels to displayrows
  }
  else
  {
    skiptop = 0;
    if (y + height > HEIGHT) renderheight = HEIGHT - y;
    else renderheight = height;
  }
  uint24_t offset = (uint24_t)(frame * ((height+7)/8) + skiptop) * width + skipleft;
  if (mode & dbmMasked)
  {
    offset += offset; // double for masked bitmaps
    width += width;
  }
  address += offset + 4; // skip non rendered pixels, width, height
  int8_t displayrow = (y >> 3) + skiptop;
  int16_t displayoffset = displayrow * WIDTH + x + skipleft;
  uint8_t yshift = bitShiftLeftUInt8(y); //shift by multiply
  uint8_t lastmask = bitShiftRightMaskUInt8(8 - height); // mask for bottom most pixels
  
  #ifdef USE_LITTLEFS
   seekData(address);
   int32_t initAddress = address;
   uint8_t *bitmapBuffer;
   int32_t pointertoBmp = 0;
   bitmapBuffer = (uint8_t *) malloc (width*(height+7)/8);
   readBytes(bitmapBuffer, width*(height+7)/8);
  #endif
  
  do{ 
    #ifdef USE_LITTLEFS
     pointertoBmp=address-initAddress;
    #else
     seekData(address);
    #endif
    
    address += width;
    mode &= ~((1 << dbfExtraRow));
    if (yshift != 1 && displayrow < (HEIGHT / 8 - 1)) mode |= (1 << dbfExtraRow);
    uint8_t rowmask = 0xFF;
    if (renderheight < 8) rowmask = lastmask;
    for (uint8_t c = 0; c < renderwidth; c++)
    {
    
      
      #ifdef USE_LITTLEFS
        uint8_t bitmapbyte = bitmapBuffer[pointertoBmp++];
      #else
        uint8_t bitmapbyte = readUnsafe();
      #endif
           
      if (mode & (1 << dbfReverseBlack)) bitmapbyte ^= rowmask;
      uint8_t maskbyte = rowmask;
      if (mode & (1 << dbfWhiteBlack)) maskbyte = bitmapbyte;
      if (mode & (1 << dbfBlack)) bitmapbyte = 0;
      uint16_t bitmap = multiplyUInt8(bitmapbyte, yshift);
      if (mode & (1 << dbfMasked))
      {
      
        
        #ifdef USE_LITTLEFS
         uint8_t tmp = bitmapBuffer[pointertoBmp++];
        #else
         uint8_t tmp = readUnsafe();
        #endif 
        
        if ((mode & dbfWhiteBlack) == 0) maskbyte = tmp;
      }
      uint16_t mask = multiplyUInt8(maskbyte, yshift);
      if (displayrow >= 0)
      {
        uint8_t pixels = bitmap;
        uint8_t display = Arduboy2Base::sBuffer[displayoffset];
        if ((mode & (1 << dbfInvert)) == 0) pixels ^= display;
        pixels &= mask;
        pixels ^= display;
        Arduboy2Base::sBuffer[displayoffset] = pixels;
      }
      if (mode & (1 << dbfExtraRow))
      {
        uint8_t display = Arduboy2Base::sBuffer[displayoffset + WIDTH];
        uint8_t pixels = bitmap >> 8;
        if ((mode & dbfInvert) == 0) pixels ^= display;
        pixels &= mask >> 8;
        pixels ^= display;
        Arduboy2Base::sBuffer[displayoffset + WIDTH] = pixels;
      }
      displayoffset++;
    }
    displayoffset += WIDTH - renderwidth;
    displayrow ++;
    renderheight -= 8;
    
    //readEnd();
    
  } while (renderheight > 0);
  
  #ifdef USE_LITTLEFS
   free(bitmapBuffer);
  #endif
}


void FX::setFrame(uint24_t frame, uint8_t repeat) //~22 bytes
{
  frameControl.start   = frame;
  frameControl.current = frame;
  frameControl.repeat  = repeat;
  frameControl.count   = repeat;
}



uint8_t FX::drawFrame() // ~66 bytes
{
  uint24_t frame = drawFrame(frameControl.current);
  uint8_t moreFrames;
  moreFrames = (frame != 0) | frameControl.count;
  if (frameControl.count > 0)
  {
    frameControl.count--;
  }
  else
  {
    frameControl.count = frameControl.repeat;
    if (!moreFrames) frame = frameControl.start;
    frameControl.current = frame;
  }
  return moreFrames;
}


uint24_t FX::drawFrame(uint24_t address) //~94 bytes
{
  FrameData f;
  for(;;)
  {
    seekData(address);
    address += 9;/*sizeof(f);*/
    f.x = readPendingUInt16();
    f.y = readPendingUInt16();
    f.bmp = readPendingUInt24();
    f.frame = readPendingUInt8();
    f.mode = readByte();
	drawBitmap(f.x, f.y, f.bmp, f.frame, f.mode);
    if (f.mode & dbmEndFrame) return address;
    if (f.mode & dbmLastFrame) return 0;
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
  seekDataArray(address, index, 0, /*sizeof(uint24_t)*/sizeof_uint24_t);
  return readPendingLastUInt24();
}


uint32_t FX::readIndexedUInt32(uint24_t address, uint8_t index){
  seekDataArray(address, index, 0, /*sizeof(uint24_t)*/sizeof_uint24_t);
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
  if (c != '\n')
  {
    drawBitmap(x, y, font.address, c, mode);
    if (mode & dcmProportional)
    {
      seekData(font.address - 256 + c);
      x += readByte();
    }
    else
    {
      x += font.width;
    }
  }
  if ((c == '\n') || (x >= cursor.wrap))
  {
    x = cursor.left;
    y += font.height;
  }
  setCursor(x,y);
}

void FX::drawString(const uint8_t* buffer){
  for(;;)
  {
    uint8_t c = *buffer++;
    if (c) drawChar(c);
    else break;
  }
}

void FX::drawString(const char* str){
  FX::drawString((const uint8_t*)str);
}

void FX::drawString(uint24_t address){
  for(;;)
  {
    seekData(address++);
    uint8_t c = readByte();
    if (c) drawChar(c);
    else break;
  }
}

void FX::drawNumber(int16_t n, int8_t digits){
  drawNumber((int32_t)n, digits);
}

void FX::drawNumber(uint16_t n, int8_t digits){
  drawNumber((uint32_t)n, digits);
}

void FX::drawNumber(int32_t n, int8_t digits){
  if (n < 0)
  {
    n = -n;
    drawChar('-');
  }
  else if (digits != 0)
  {
    drawChar(' ');
  }
  drawNumber((uint32_t)n, digits);
}

void FX::drawNumber(uint32_t n, int8_t digits) //
{
  uint8_t buf[33]; //max 32 digits + terminator
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
  for(;;)
  {
    uint8_t c = *buffer++;
    if (c) drawCharOld(x, y, c, color, !color, 1);
    else break;
    x+=6;
  }
}


void FX::drawPixelOld(int16_t x, int16_t y, uint8_t color){
  if (x < 0 || x > (WIDTH-1) || y < 0 || y > (HEIGHT-1)){
    return;
  }
  uint8_t row = (uint8_t)y / 8;
  if (color){
    Arduboy2Base::sBuffer[(row*WIDTH) + (uint8_t)x] |=   _BV((uint8_t)y % 8);
  }
  else{
    Arduboy2Base::sBuffer[(row*WIDTH) + (uint8_t)x] &= ~ _BV((uint8_t)y % 8);
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
  
  const uint8_t* bitmap =
    &font5x7local[c * characterWidth * ((characterHeight + 8 - 1) / 8)];
  for (uint8_t i = 0; i < fullCharacterWidth; i++){
    uint8_t column;
    if (characterHeight <= 8){
      column = (i < characterWidth) ? pgm_read_byte(bitmap++) : 0;}
    else{
      column = 0;}

    for (uint8_t j = 0; j < characterHeight; j++){
      if (characterHeight > 8){
        if ((j % 8 == 0) && (i < characterWidth)){
          column = pgm_read_byte(bitmap++);
        }
      }
      uint8_t pixelIsSet = column & 0x01;
      if (pixelIsSet || drawBackground){
        for (uint8_t a = 0; a < size; a++){
          for (uint8_t b = 0; b < size; b++){
            drawPixelOld(x + (i * size) + a, y + (j * size) + b,
                      pixelIsSet ? color : bg);
          }
        }
      }
      column >>= 1;
    }

    if (drawBackground){
      for (uint8_t j = characterHeight; j < fullCharacterHeight; j++){
        for (uint8_t a = 0; a < size; a++){
          for (uint8_t b = 0; b < size; b++){
            drawPixelOld(x + (i * size) + a, y + (j * size) + b, bg);
          }
        }
      }
    }
  }
}

