#ifndef ARDUBOYFX_H
#define ARDUBOYFX_H

#define EEPROMWRITEOFFSET 34

/*
 * =========================================================================
 *               ArduboyFX Library Port for ESPboy
 * =========================================================================
 *
 * QUICK CONFIGURATION & UTILITIES GUIDE
 * -------------------------------------
 * Select one of the three operational modes depending on your game's 
 * original FX asset size and compression constraints (PROGMEM limit < 700 KB):
 *
 * -------------------------------------------------------------------------
 * MODE 1: DIRECT PROGMEM MODE (Original fxdata.bin < 700 KB)
 * -------------------------------------------------------------------------
 * - Uncompressed FX data is stored directly in Flash memory as `const uint8_t fxdta[]`.
 * - Offers maximum performance with zero decompression overhead.
 * - Workflow:
 *     1. Keep `USE_LITTLEFS` and `USE_LZSS_PACKING` commented out.
 *     2. Convert `fxdata.bin` to a C-array using `bin2c.py`:
 *        python bin2c.py fxdata.bin fxdta.cpp
 *     3. Include the generated `fxdta.cpp` in your project.
 *
 * -------------------------------------------------------------------------
 * MODE 2: PROGMEM RLE -> LITTLEFS UNPACKING (Original fxdata.bin ~700-1000 KB+)
 * -------------------------------------------------------------------------
 * - RLE-compressed data is stored in PROGMEM (< 700 KB).
 * - On first boot, the library automatically unpacks the full uncompressed 
 *   asset file onto LittleFS (`/fxdta.bin`).
 * - Subsequent game runs read uncompressed assets directly from LittleFS.
 * - Workflow:
 *     1. Define `#define USE_LITTLEFS` and `#define USE_RLE_COMPRESSION` below.
 *     2. Keep `USE_LZSS_PACKING` commented out.
 *     3. Compress `fxdata.bin` using `rle_compress.py`:
 *        python rle_compress.py fxdata.bin fxdta_rle.bin
 *     4. Copy the generated defines from python output into this header:
 *        #define UNPACKED_FILE_SIZE <bytes_before_compression>
 *        #define RLE_FILE_SIZE      <bytes_after_compression>
 *     5. Convert `fxdta_rle.bin` to a C-array using `bin2c.py`:
 *        python bin2c.py fxdta_rle.bin fxdta.cpp
 *     6. Include the generated `fxdta.cpp` in your project.
 *
 * -------------------------------------------------------------------------
 * MODE 3: PAGED DYNAMIC LZSS MODE (Original fxdata.bin > 1000 KB)
 * -------------------------------------------------------------------------
 * - Assets are compressed into 4 KB pages using LZSS (`lzss_compress.py`) 
 *   and stored in PROGMEM (< 700 KB).
 * - Pages are decompressed dynamically on-the-fly into RAM at runtime without 
 *   using LittleFS or modifying `fxdata.h` addresses.
 * - Workflow:
 *     1. Uncomment `#define USE_LZSS_PACKING` below.
 *     2. Keep `USE_LITTLEFS` commented out.
 *     3. Compress `fxdata.bin` using `lzss_compress.py`:
 *        python lzss_compress.py fxdata.bin fxdata_paged.bin
 *     4. Convert `fxdata_paged.bin` to a C-array using `bin2c.py`:
 *        python bin2c.py fxdata_paged.bin fxdta.cpp
 *     5. Include the generated `fxdta.cpp` in your project.
 *
 * =========================================================================
 */

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//     DEFINE ArduboyFX work MODE here according to description above
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//#define USE_LITTLEFS 
//#define USE_RLE_COMPRESSION
//#define USE_LZSS_PACKING



// -------------------------------------------------------------------------
// Mode 2 RLE Size Defines (Paste python output here for Mode 2)
// -------------------------------------------------------------------------
//#define RLE_FILE_SIZE 233968 // for EXOLON
//#define UNPACKED_FILE_SIZE 802913 // for EXOLON
//#define RLE_FILE_SIZE 288049 // for PRINCE OF ARABIA v1.4
//#define UNPACKED_FILE_SIZE 674622 // for PRINCE OF ARABIA v1.4

// Enable Serial Debug Output
//#define DEBUG_INFO_ON



#ifdef USE_RLE_COMPRESSION
  #define USE_LITTLEFS 
#endif

#ifndef USE_LITTLEFS
  #undef USE_RLE_COMPRESSION
#endif

#include <stdint.h>
#include <stddef.h>
#include <Arduino.h>
#include <Arduboy2.h>

#ifdef USE_LITTLEFS
  #include <LittleFS.h>
#endif

// drawbitmap bit flags (used by modes below and internally)
constexpr uint8_t dbfWhiteBlack   = 0; // bitmap is used as mask
constexpr uint8_t dbfInvert       = 1; // bitmap is exclusive or-ed with display
constexpr uint8_t dbfBlack        = 2; // bitmap will be blackened
constexpr uint8_t dbfReverseBlack = 3; // reverses bitmap data
constexpr uint8_t dbfMasked       = 4; // bitmap contains mask data
constexpr uint8_t dbfFlip         = 5; // mirror bitmap
constexpr uint8_t dbfExtraRow     = 7; // ignored (internal use)
constexpr uint8_t dbfEndFrame     = 6; // last bitmap image of a frame
constexpr uint8_t dbfLastFrame    = 7; // last bitmap image of the last frame

// drawBitmap modes with same behaviour as Arduboy library drawBitmap modes
constexpr uint8_t dbmBlack   = (1 << dbfReverseBlack) |
                               (1 << dbfBlack) |
                               (1 << dbfWhiteBlack);

constexpr uint8_t dbmWhite   = (1 << dbfWhiteBlack);

constexpr uint8_t dbmInvert  = (1 << dbfInvert);
                                                        
constexpr uint8_t dbmFlip    = (1 << dbfFlip);
                                                        
// additional drawBitmap modes
constexpr uint8_t dbmNormal    = 0;
constexpr uint8_t dbmOverwrite = 0;

constexpr uint8_t dbmReverse   = (1 << dbfReverseBlack);
constexpr uint8_t dbmMasked    = (1 << dbfMasked);
constexpr uint8_t dbmEndFrame  = (1 << dbfEndFrame);
constexpr uint8_t dbmLastFrame = (1 << dbfLastFrame);

// drawChar bit flags
constexpr uint8_t dcfWhiteBlack   = 0;
constexpr uint8_t dcfInvert       = 1;
constexpr uint8_t dcfBlack        = 2;
constexpr uint8_t dcfReverseBlack = 3;
constexpr uint8_t dcfMasked       = 4;
constexpr uint8_t dcfProportional = 5;

// draw Font character modes
constexpr uint8_t dcmBlack   = (1 << dcfReverseBlack) |
                               (1 << dcfBlack) |
                               (1 << dcfWhiteBlack);

constexpr uint8_t dcmWhite   = (1 << dcfWhiteBlack);
constexpr uint8_t dcmInvert  = (1 << dcfInvert);
constexpr uint8_t dcmNormal  = 0;
constexpr uint8_t dcmOverwrite = 0;
constexpr uint8_t dcmReverse = (1 << dcfReverseBlack);
constexpr uint8_t dcmMasked  = (1 << dcfMasked);
constexpr uint8_t dcmProportional = (1 << dcfProportional);

#define uint24_t uint32_t
#define sizeof_uint24_t 3

struct JedecID
{
  uint8_t manufacturer;
  uint8_t device;
  uint8_t size;
};

struct FXAddress
{
  uint16_t page;
  uint8_t  offset;
};

struct Font
{
  uint24_t address;
  uint8_t  mode;
  uint8_t  width;
  uint8_t  height;
};

struct Cursor
{
  int16_t x;
  int16_t y;
  int16_t left;
  int16_t wrap;
};

struct FrameControl
{
  uint24_t start;
  uint24_t current;
  uint8_t repeat;
  uint8_t count;
};

struct FrameData
{
  int16_t  x;
  int16_t  y;
  uint24_t bmp;
  uint8_t  frame;
  uint8_t  mode;
};

class FX
{
  public:
    static void drawPixelOld(int16_t x, int16_t y, uint8_t color = 1);
    static void Rle_Decode(unsigned char *inbuf, uint32_t inSize);
    static void drawCharOld(int16_t x, int16_t y, uint8_t c, uint8_t color, uint8_t bg, uint8_t size);
    static void drawStringOld(int16_t x, int16_t y, uint8_t color, const char* buffer);
    static void readAudioBytes(uint24_t address, uint8_t* buffer, size_t length);
  
    [[gnu::always_inline]] static inline void enableOLED() {};
    [[gnu::always_inline]] static inline void disableOLED() {};
    [[gnu::always_inline]] static inline void enable() {};
    [[gnu::always_inline]] static inline void disable() {};
    [[gnu::always_inline]] static inline void wait() {};

    static uint8_t writeByte(uint8_t data);

    [[gnu::always_inline]] static inline void writeByteBeforeWait(uint8_t data){ writeByte(data); }
    [[gnu::always_inline]] static inline void writeByteAfterWait(uint8_t data){ writeByte(data); }

    static uint8_t readByte();
    static void displayPrefetch(uint24_t address, uint8_t* target, uint16_t len, bool clear);
    static void display();
    static void display(bool clear);

    static void begin();
    static void begin(uint16_t programDataPage);
    static void begin(uint16_t datapage, uint16_t savepage);

    static void readJedecID(JedecID & id);
    static void readJedecID(JedecID* id);

    static bool detect();
    static void noFXReboot();

    [[gnu::noinline]] static void seekData(uint24_t address);

    template<typename Type>
    static void seekArrayElement(uint24_t address, uint8_t index)
    {
      static_assert(sizeof(Type) > 0, "Cannot use a Type with a size of 0.");
      seekData(address + (index * sizeof(Type)));
    }

    template<typename Type>
    static void seekArrayElementMember(uint24_t address, uint8_t index, size_t offset)
    {
      static_assert(sizeof(Type) > 0, "Cannot use a Type with a size of 0.");
      seekData(address + ((index * sizeof(Type)) + offset));
    }

    [[gnu::noinline]] static void seekDataArray(uint24_t address, uint8_t index, uint8_t offset, uint8_t elementSize);
    [[gnu::noinline]] static void seekSave(uint24_t address);

    [[gnu::always_inline]] static inline uint8_t readUnsafe() { return readByte(); };
    [[gnu::always_inline]] static inline uint8_t readUnsafeEnd() { return readByte(); };

    [[gnu::noinline]] static uint8_t readPendingUInt8();
    [[gnu::noinline]] static uint8_t readPendingLastUInt8();
    [[gnu::noinline]] static uint16_t readPendingUInt16();
    [[gnu::noinline]] static uint16_t readPendingLastUInt16();
    static uint24_t readPendingUInt24();
    static uint24_t readPendingLastUInt24();
    static uint32_t readPendingUInt32();
    static uint32_t readPendingLastUInt32();

    template<typename Type>
    static void readObject(Type & object)
    {
      readBytes(reinterpret_cast<uint8_t *>(&object), sizeof(object));
    }

    static void readBytes(uint8_t* buffer, size_t length);
    static void readBytesSave(uint8_t* buffer, size_t length);
    static void readBytesEnd(uint8_t* buffer, size_t length);
    static void safeCommit();

    [[gnu::noinline]] static uint8_t readEnd();

    template<typename Type>
    static void readDataObject(uint24_t address, Type & object)
    {
      readDataBytes(address, reinterpret_cast<uint8_t *>(&object), sizeof(object));
    }

    static void readDataBytes(uint24_t address, uint8_t* buffer, size_t length);

    template<typename Type>
    static void readSaveObject(uint24_t address, Type & object)
    {
      readSaveBytes(address, reinterpret_cast<uint8_t *>(&object), sizeof(object));
    }

    static void readSaveBytes(uint24_t address, uint8_t* buffer, size_t length);

    template<typename Type>
    static uint8_t loadGameState(Type & object)
    {
      return loadGameState((uint8_t*)(&object), sizeof(object));
    }

    [[gnu::noinline]] static uint8_t loadGameState(uint8_t* gameState, size_t size);

    template<typename Type>
    static void saveGameState(const Type & object)
    {
      saveGameState(reinterpret_cast<const uint8_t *>(&object), sizeof(object));
    }

    [[gnu::noinline]] static void saveGameState(const uint8_t* gameState, size_t size);

    static void eraseSaveBlock(uint16_t page);
    static void writeSavePage(uint16_t page, uint8_t* buffer);

    [[gnu::noinline]] static void drawBitmap(int16_t x, int16_t y, uint24_t address, uint8_t frame, uint8_t mode);
    [[gnu::noinline]] static void setFrame(uint24_t frame, uint8_t repeat);
    static uint8_t drawFrame();
    [[gnu::noinline]] static uint24_t drawFrame(uint24_t address);

    static void readDataArray(uint24_t address, uint8_t index, uint8_t offset, uint8_t elementSize, uint8_t* buffer, size_t length);

    static uint8_t readIndexedUInt8(uint24_t address, uint8_t index);
    static uint16_t readIndexedUInt16(uint24_t address, uint8_t index);
    static uint24_t readIndexedUInt24(uint24_t address, uint8_t index);
    static uint32_t readIndexedUInt32(uint24_t address, uint8_t index);

    /* Draw character functions */
    static void setFont(uint24_t address, uint8_t mode);
    static void setFontMode(uint8_t mode);
    static void setCursor(int16_t x, int16_t y);
    static void setCursorX(int16_t x);
    static void setCursorY(int16_t y);
    static void setCursorRange(int16_t left, int16_t wrap);
    static void setCursorLeft(int16_t x);
    static void setCursorWrap(int16_t y);
    static void drawChar(uint8_t c);
    static void drawString(const uint8_t* buffer);
    static void drawString(const char* str);
    static void drawString(uint24_t address);

    static void drawNumber(int16_t n, int8_t digits = 0);
    static void drawNumber(uint16_t n, int8_t digits = 0);
    static void drawNumber(int32_t n, int8_t digits = 0);
    static void drawNumber(uint32_t n, int8_t digits = 0);

    /* general optimized functions */
    [[gnu::always_inline]] static inline uint16_t multiplyUInt8 (uint8_t a, uint8_t b) { return (a * b); }
    [[gnu::always_inline]] static inline uint8_t bitShiftLeftUInt8(uint8_t bit) { return 1 << (bit & 7); }
    [[gnu::always_inline]] static inline uint8_t bitShiftRightUInt8(uint8_t bit) { return 0x80 >> (bit & 7); }
    [[gnu::always_inline]] static inline uint8_t bitShiftLeftMaskUInt8(uint8_t bit) { return (0xFF << (bit & 7)) & 0xFF; }
    [[gnu::always_inline]] static inline uint8_t bitShiftRightMaskUInt8(uint8_t bit) { return 0xFF >> (bit & 7); }
    [[gnu::always_inline]] static inline int16_t fastDiv8(int16_t i) { return i >> 3; };
    [[gnu::always_inline]] static inline uint16_t fastDiv8(uint16_t i) { return i >> 3; };

    static uint16_t programDataPage;
    static uint16_t programSavePage;
    static Font     font;
    static Cursor   cursor;
    static FrameControl frameControl;
};

#endif