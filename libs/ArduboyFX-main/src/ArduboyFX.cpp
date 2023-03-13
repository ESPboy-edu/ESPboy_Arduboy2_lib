//TO DO void FX::displayPrefetch


#include "ArduboyFX.h"
//#include <wiring.c>

uint32_t globalAddress=0;
extern uint8_t fxdta[];


uint16_t FX::programDataPage = 0; // program read only data location in flash memory
uint16_t FX::programSavePage = 0; // program read and write data location in flash memory
Font     FX::font;
Cursor   FX::cursor = {0,0,0,WIDTH};

FrameControl FX::frameControl;


uint8_t FX::writeByte(uint8_t data){
  return data;}


uint8_t FX::readByte(){
  uint8_t result = pgm_read_byte(&fxdta[globalAddress]);
  globalAddress++;
  return result;
}


void FX::begin(){}
void FX::begin(uint16_t developmentDataPage){begin();}
void FX::begin(uint16_t developmentDataPage, uint16_t developmentSavePage){begin();}

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

void FX::noFXReboot() {ESP.reset();}


void FX::seekCommand(uint24_t address){
  globalAddress = address;
}


void FX::seekData(uint24_t address){
  address += (uint24_t)programDataPage << 8;
  seekCommand(address);
}


void FX::seekDataArray(uint24_t address, uint8_t index, uint8_t offset, uint8_t elementSize){
  address += index * elementSize + offset;
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
  return ((uint16_t)readByte() << 8) | (uint16_t)readByte();
}


uint16_t FX::readPendingLastUInt16(){
  return ((uint16_t)readByte() << 8) | readByte();
}


uint24_t FX::readPendingUInt24(){
  return ((uint24_t)readPendingUInt16() << 8) | readByte();
}


uint24_t FX::readPendingLastUInt24(){
  return ((uint24_t)readPendingUInt16() << 8) | readByte();
}


uint32_t FX::readPendingUInt32(){
  return ((uint32_t)readPendingUInt16() << 16) | readPendingUInt16();
}


uint32_t FX::readPendingLastUInt32(){
  return ((uint32_t)readPendingUInt16() << 16) | readPendingLastUInt16();
}


void FX::readBytes(uint8_t* buffer, size_t length){
  for (size_t i = 0; i < length; i++)
    buffer[i] = readPendingUInt8();
}


void FX::readBytesEnd(uint8_t* buffer, size_t length){
  readBytes(buffer, length);
}


uint8_t FX::readEnd(){
  return readByte();
}


void FX::readDataBytes(uint24_t address, uint8_t* buffer, size_t length){
  seekData(address);
  readBytesEnd(buffer, length);
}


void FX::readSaveBytes(uint24_t address, uint8_t* buffer, size_t length){
  readDataBytes(address, buffer, length);
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
    seekCommand((uint24_t)(programSavePage << 8) + addr);
    do
    {
      writeByte(*gameState++);
      if (--size == 0) break;
    }
    while ((uint8_t)++addr); // write bytes until end of a page
  }
}


void  FX::eraseSaveBlock(uint16_t page){
  seekCommand((uint24_t)(programSavePage + page) << 8);
}


void FX::writeSavePage(uint16_t page, uint8_t* buffer){
  seekCommand((uint24_t)(programSavePage + page) << 8);
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
  //uint24_t offset = (uint24_t)(frame * (fastDiv8(height+(uint16_t)7)) + skiptop) * width + skipleft;
  uint24_t offset = (uint24_t)(frame * ((height+(uint16_t)7)/8) + skiptop) * width + skipleft;
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
  do
  {
    seekData(address);
    address += width;
    mode &= ~((1 << dbfExtraRow));
    if (yshift != 1 && displayrow < (HEIGHT / 8 - 1)) mode |= (1 << dbfExtraRow);
    uint8_t rowmask = 0xFF;
    if (renderheight < 8) rowmask = lastmask;
    for (uint8_t c = 0; c < renderwidth; c++)
    {
      uint8_t bitmapbyte = readUnsafe();
      if (mode & (1 << dbfReverseBlack)) bitmapbyte ^= rowmask;
      uint8_t maskbyte = rowmask;
      if (mode & (1 << dbfWhiteBlack)) maskbyte = bitmapbyte;
      if (mode & (1 << dbfBlack)) bitmapbyte = 0;
      uint16_t bitmap = multiplyUInt8(bitmapbyte, yshift);
      if (mode & (1 << dbfMasked))
      {
        uint8_t tmp = readUnsafe();
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
    readEnd();
  } while (renderheight > 0);
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
  seekData(address);
  address += sizeof(f);
  for(;;)
  {
    f.x = readPendingUInt16();
    f.y = readPendingUInt16();
    f.bmp = readPendingUInt24();
    f.frame = readPendingUInt8();
    f.mode = readEnd();
    drawBitmap(f.x, f.y, f.bmp, f.frame, f.mode);
    if (f.mode & dbmEndFrame) return address;
    if (f.mode & dbmLastFrame) return 0;
  }
}


void FX::readDataArray(uint24_t address, uint8_t index, uint8_t offset, uint16_t elementSize, uint8_t* buffer, size_t length){
  seekDataArray(address, index, offset, elementSize);
  readBytesEnd(buffer, length);
}


uint8_t FX::readIndexedUInt8(uint24_t address, uint8_t index){
  seekDataArray(address, index, 0, sizeof(uint8_t));
  return readEnd();
}


uint16_t FX::readIndexedUInt16(uint24_t address, uint8_t index){
  seekDataArray(address, index, 0, sizeof(uint16_t));
  return readPendingLastUInt16();
}


uint24_t FX::readIndexedUInt24(uint24_t address, uint8_t index){
  seekDataArray(address, index, 0, sizeof(uint24_t));
  return readPendingLastUInt24();
}


uint32_t FX::readIndexedUInt32(uint24_t address, uint8_t index){
  seekDataArray(address, index, 0, sizeof(uint24_t));
  return readPendingLastUInt32();
}


void FX::displayPrefetch(uint24_t address, uint8_t* target, uint16_t len, bool clear){
 uint8_t targetdata;
 uint8_t displaydata;
  seekData(address);
  uint8_t *ptr = Arduboy2::sBuffer;
  while (true) {
    displaydata = *ptr;
    targetdata = readByte();
    //SPDR = displaydata;
    if (clear) displaydata = 0;
    *ptr++ = displaydata;
    if (--len >= 0) *target++ = targetdata;
    if (ptr >= Arduboy2::sBuffer + WIDTH * HEIGHT / 8) break;
  }
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
      x += readEnd();
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
    uint8_t c = readEnd();
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
