
#define USE_nbSPI //accelerates gfx but less hardware compatibility
#define MAX_PALETTES 37
#define DEFAULT_PALETTE 11
#define Y_SCALE_PRESET 0

//#pragma GCC optimize ("-O3")
//#pragma GCC push_options

/*

    When using L4_Triplane, you can define one of the following macros to
    convert to L3 while retaining the plane behavior of L4_Triplane:
    - ABG_L3_CONVERT_LIGHTEN
        Convert light gray to white and dark gray to gray
    - ABG_L3_CONVERT_MIX
        Convert both light gray and dark gray to gray
    - ABG_L3_CONVERT_DARKEN
        Convert light gray to gray and dark gray to black

Default Template Configuration:
    
    ArduboyGBase a;
    ArduboyG     a;

    Both are configured with:
        ABG_Mode::L3
        ABG_Flags::None

Custom Template Configuration:

    ArduboyGBase_Config<ABG_Mode::L3, ABG_Flags:None> a;
    ArduboyG_Config    <ABG_Mode::L3, ABG_Flags:None> a;
                    
Example Usage:

    #define ABG_IMPLEMENTATION
    #include "ArduboyG.h"

    ArduboyG a;

    int16_t x, y;

    void update()
    {
        // Handle input and update game state here.
        if(a.pressed(UP_BUTTON))    --y;
        if(a.pressed(DOWN_BUTTON))  ++y;
        if(a.pressed(LEFT_BUTTON))  --x;
        if(a.pressed(RIGHT_BUTTON)) ++x;
    }


    void render()
    {
        // Draw your game graphics here.
        a.setCursor(20, 28);
        a.setTextColor(WHITE);
        a.print(F("Hello "));
        a.setTextColor(DARK_GRAY);
        a.print(F("ArduboyG!"));
        a.fillRect(x +  0, y, 5, 15, WHITE);
        a.fillRect(x +  5, y, 5, 15, LIGHT_GRAY);
        a.fillRect(x + 10, y, 5, 15, DARK_GRAY);
    }

    void setup()
    {
        a.begin();
        
        // Initialize your game state here.
        
        // This method kicks off the frame ISR that handles refreshing
        // the screen. Usually you would call this at the end of setup().
        a.startGray();
    }

    void loop()
    {
        a.waitForNextPlane();
        if(a.needsUpdate())
            update();
        render();
    }

*/

#pragma once

#include <Arduboy2.h>
//#include "nbSPI.h"
#include "palettesCollection.h"

#ifdef USE_nbSPI
extern bool nbSPI_isBusy();
extern void nbSPI_writeBytes(uint8_t *data, uint16_t size);
#endif

extern ESPboyInit myESPboy;

#define VERT_OFFSET     20

#ifdef USE_nbSPI
        #define SWPLH(x) ((x>>8)|(x<<8))
#else
        #define SWPLH(x) x
#endif

#ifndef DEFAULT_PALETTE
 #define DEFAULT_PALETTE 0 //check list of palettes in "palettesCollection.h"
#endif

  PROGMEM const static uint8_t decodePaletteL4C[8] = {3, 2, 1, 0, 1, 0, 0, 0};
  PROGMEM const static uint8_t decodePaletteL4T[8] = {3, 2, 2, 1, 2, 1, 1, 0};
  PROGMEM const static uint8_t decodePaletteL4L[8] = {3, 4, 4, 0, 4, 4, 4, 0};
  PROGMEM const static uint8_t decodePaletteL4D[8] = {3, 3, 4, 4, 4, 4, 4, 0};
  PROGMEM const static uint8_t decodePaletteL4M[8] = {3, 4, 4, 4, 4, 4, 4, 0};
  PROGMEM const static uint8_t decodePaletteL3 [8] = {3, 4, 4, 0, 4, 4, 4, 4};

  PROGMEM const static uint8_t  *paletteDecodeTable[] = {decodePaletteL4C, decodePaletteL4T, decodePaletteL4L, decodePaletteL4D, decodePaletteL4M, decodePaletteL3};

  static uint16_t currentPalette[8];
  static uint8_t paletteDecodeTableIndex = 0;
  static int16_t paletteIndex = DEFAULT_PALETTE;
  
  static uint8_t *plane0, *plane1;
  static uint16_t *oBuffer1, *oBuffer2, *oBuffer;
  
  static uint8_t *b;
  static bool arduboyYscaleFlag = Y_SCALE_PRESET;
  static uint32_t frameRateDelayMs = 1000/200, frameRateMillisPrev;


#undef BLACK
#undef WHITE
constexpr uint8_t BLACK      = 0;
constexpr uint8_t DARK_GRAY  = 1;
constexpr uint8_t DARK_GREY  = 1;
constexpr uint8_t GRAY       = 1;
constexpr uint8_t GREY       = 1;
constexpr uint8_t LIGHT_GRAY = 2;
constexpr uint8_t LIGHT_GREY = 2;
constexpr uint8_t WHITE      = 3;

    
enum class ABG_Mode : uint8_t
{
    L4_Contrast,
    L4_Triplane,
    L3,
    
    Default = L3,
};

struct ABG_Flags{
    enum{
        None = 0,
        Default = 0,
    };
};

#ifdef __GNUC__
#define ABG_NOT_SUPPORTED __attribute__((error( \
    "This method cannot be called when using ArduboyG.")))
#else
// will still cause linker error
#define ABG_NOT_SUPPORTED
#endif


#if !defined(ABG_UPDATE_EVERY_N_DEFAULT)
#define ABG_UPDATE_EVERY_N_DEFAULT 1
#endif

namespace abg_detail
{

static constexpr uint8_t num_planes(ABG_Mode mode)
{
    return
        mode == ABG_Mode::L4_Contrast ? 2 :
        mode == ABG_Mode::L4_Triplane ? 3 :
        mode == ABG_Mode::L3          ? 2 :
        1;
}


extern uint8_t  current_plane;
extern bool     update_flag;
extern uint8_t  update_every_n;
extern uint8_t  update_every_n_count;
 
template<
    class    BASE,
    ABG_Mode MODE,
    uint32_t FLAGS
>

struct ArduboyG_Common : public BASE
{

    static uint8_t justPressedButtons() {
        return (~Arduboy2Base::previousButtonState & Arduboy2Base::currentButtonState);
    }
    
    static uint8_t pressedButtons() {
        return Arduboy2Base::currentButtonState;
    }
    
    static void startGray(){
      
        if(!arduboyYscaleFlag){          
            myESPboy.tft.setAddrWindow(0, VERT_OFFSET, WIDTH, HEIGHT);
            }
        else{
            myESPboy.tft.setAddrWindow(0, 0, WIDTH, HEIGHT*2);
            }
            
            plane0 =  (uint8_t *) malloc(128*64/8);
            plane1 =  (uint8_t *) malloc(128*64/8);
            memset(plane0, 0, 128*64/8);
            memset(plane0, 0, 128*64/8);
		        oBuffer1 = (uint16_t *)malloc(WIDTH*2*16*sizeof(uint16_t));
            oBuffer2 = (uint16_t *)malloc(WIDTH*2*16*sizeof(uint16_t));
            oBuffer = oBuffer1;
            b = Arduboy2Base::getBuffer();
            memset(b, 0, 128*64/8);
    
#ifdef ABG_L3_CONVERT_LIGHTEN
            paletteDecodeTableIndex = 2;
#else
#ifdef ABG_L3_CONVERT_DARKEN
            paletteDecodeTableIndex = 3;
#else
#ifdef ABG_L3_CONVERT_MIX
            paletteDecodeTableIndex = 4;
#else
            if(MODE == ABG_Mode::L4_Triplane) paletteDecodeTableIndex = 1;
            else
            if(MODE == ABG_Mode::L4_Contrast) paletteDecodeTableIndex = 1;
            else 
            paletteDecodeTableIndex = 5;
#endif
#endif
#endif
          for(uint8_t i=0; i<8; i++)
                         currentPalette[i] = SWPLH(
                                  myESPboy.tft.color24to16(
                                    pgm_read_dword(&
                                      palettesCollection[paletteIndex][pgm_read_byte(&(paletteDecodeTable[paletteDecodeTableIndex])[i])]
                                  )
                                    )
                                );
       
      while(currentPlane() != num_planes(MODE)-1) waitForNextPlane();
      
    }  
    
    
    //static void setFrameRate(uint8_t fr){
    //  frameRateDelayMs = 1000/fr;
    //}
    
    
    static void startGrey() {startGray();}
    
    // use this method to adjust contrast when using ABGMode::L4_Contrast
    static void setContrast(uint8_t f) {}
    
    static void setUpdateEveryN(uint8_t num, uint8_t denom = 1){
      update_every_n = num;
    }

    
    static void setUpdateHz(uint8_t hz) {}

    static void drawBitmap(
        int16_t x, int16_t y,
        uint8_t const* bitmap,
        uint8_t w, uint8_t h,
        uint8_t color = WHITE)
    {
        Arduboy2Base::drawBitmap(x, y, bitmap, w, h, planeColor(current_plane, color));
    }
    
    template<uint8_t PLANE>
    static void drawBitmap(
        int16_t x, int16_t y,
        uint8_t const* bitmap,
        uint8_t w, uint8_t h,
        uint8_t color = WHITE)
    {
        Arduboy2Base::drawBitmap(x, y, bitmap, w, h, planeColor<PLANE>(color));
    }
    
    static void drawSlowXYBitmap(
        int16_t x, int16_t y,
        uint8_t const* bitmap,
        uint8_t w, uint8_t h,
        uint8_t color = WHITE)
    {
        Arduboy2Base::drawSlowXYBitmap(x, y, bitmap, w, h, planeColor(current_plane, color));
    }
    
    template<uint8_t PLANE>
    static void drawSlowXYBitmap(
        int16_t x, int16_t y,
        uint8_t const* bitmap,
        uint8_t w, uint8_t h,
        uint8_t color = WHITE)
    {
        Arduboy2Base::drawSlowXYBitmap(x, y, bitmap, w, h, planeColor<PLANE>(color));
    }
    
    static void drawCompressed(
        int16_t sx, int16_t sy,
        uint8_t const* bitmap,
        uint8_t color = WHITE)
    {
        Arduboy2Base::drawCompressed(sx, sy, bitmap, planeColor(current_plane, color));
    }
    
    template<uint8_t PLANE>
    static void drawCompressed(
        int16_t sx, int16_t sy,
        uint8_t const* bitmap,
        uint8_t color = WHITE)
    {
        Arduboy2Base::drawCompressed(sx, sy, bitmap, planeColor<PLANE>(color));
    }
    
    static void drawPixel(
        int16_t x, int16_t y,
        uint8_t color = WHITE)
    {
        Arduboy2Base::drawPixel(x, y, planeColor(current_plane, color));
    }
    
    template<uint8_t PLANE>
    static void drawPixel(
        int16_t x, int16_t y,
        uint8_t color = WHITE)
    {
        Arduboy2Base::drawPixel(x, y, planeColor<PLANE>(color));
    }
    
    static void drawFastHLine(
        int16_t x, int16_t y,
        uint8_t w,
        uint8_t color = WHITE)
    {
        Arduboy2Base::drawFastHLine(x, y, w, planeColor(current_plane, color));
    }
    
    template<uint8_t PLANE>
    static void drawFastHLine(
        int16_t x, int16_t y,
        uint8_t w,
        uint8_t color = WHITE)
    {
        Arduboy2Base::drawFastHLine(x, y, w, planeColor<PLANE>(color));
    }
    
    static void drawFastVLine(
        int16_t x, int16_t y,
        uint8_t h,
        uint8_t color = WHITE)
    {
        Arduboy2Base::drawFastVLine(x, y, h, planeColor(current_plane, color));
    }
    
    template<uint8_t PLANE>
    static void drawFastVLine(
        int16_t x, int16_t y,
        uint8_t h,
        uint8_t color = WHITE)
    {
        Arduboy2Base::drawFastVLine(x, y, h, planeColor<PLANE>(color));
    }
    
    static void drawLine(
        int16_t x0, int16_t y0,
        int16_t x1, int16_t y1,
        uint8_t color = WHITE)
    {
        Arduboy2Base::drawLine(x0, y0, x1, y1, planeColor(current_plane, color));
    }
    
    template<uint8_t PLANE>
    static void drawLine(
        int16_t x0, int16_t y0,
        int16_t x1, int16_t y1,
        uint8_t color = WHITE)
    {
        Arduboy2Base::drawLine(x0, y0, x1, y1, planeColor<PLANE>(color));
    }
    
    static void drawCircle(
        int16_t x0, int16_t y0,
        uint8_t r,
        uint8_t color = WHITE)
    {
        Arduboy2Base::drawCircle(x0, y0, r, planeColor(current_plane, color));
    }
    
    template<uint8_t PLANE>
    static void drawCircle(
        int16_t x0, int16_t y0,
        uint8_t r,
        uint8_t color = WHITE)
    {
        Arduboy2Base::drawCircle(x0, y0, r, planeColor<PLANE>(color));
    }
    
    static void drawTriangle(
        int16_t x0, int16_t y0,
        int16_t x1, int16_t y1,
        int16_t x2, int16_t y2,
        uint8_t color = WHITE)
    {
        Arduboy2Base::drawTriangle(x0, y0, x1, y1, x2, y2, planeColor(current_plane, color));
    }
    
    template<uint8_t PLANE>
    static void drawTriangle(
        int16_t x0, int16_t y0,
        int16_t x1, int16_t y1,
        int16_t x2, int16_t y2,
        uint8_t color = WHITE)
    {
        Arduboy2Base::drawTriangle(x0, y0, x1, y1, x2, y2, planeColor<PLANE>(color));
    }
    
    static void drawRect(
        int16_t x, int16_t y,
        uint8_t w, uint8_t h,
        uint8_t color = WHITE)
    {
        Arduboy2Base::drawRect(x, y, w, h, planeColor(current_plane, color));
    }
    
    template<uint8_t PLANE>
    static void drawRect(
        int16_t x, int16_t y,
        uint8_t w, uint8_t h,
        uint8_t color = WHITE)
    {
        Arduboy2Base::drawRect(x, y, w, h, planeColor<PLANE>(color));
    }
    
    static void drawRoundRect(
        int16_t x, int16_t y,
        uint8_t w, uint8_t h,
        uint8_t r,
        uint8_t color = WHITE)
    {
        Arduboy2Base::drawRoundRect(x, y, w, h, r, planeColor(current_plane, color));
    }

    template<uint8_t PLANE>
    static void drawRoundRect(
        int16_t x, int16_t y,
        uint8_t w, uint8_t h,
        uint8_t r,
        uint8_t color = WHITE)
    {
        Arduboy2Base::drawRoundRect(x, y, w, h, r, planeColor<PLANE>(color));
    }
    
    static void fillCircle(
        int16_t x0, int16_t y0,
        uint8_t r,
        uint8_t color = WHITE)
    {
        Arduboy2Base::fillCircle(x0, y0, r, planeColor(current_plane, color));
    }
    
    template<uint8_t PLANE>
    static void fillCircle(
        int16_t x0, int16_t y0,
        uint8_t r,
        uint8_t color = WHITE)
    {
        Arduboy2Base::fillCircle(x0, y0, r, planeColor<PLANE>(color));
    }
    
    static void fillTriangle(
        int16_t x0, int16_t y0,
        int16_t x1, int16_t y1,
        int16_t x2, int16_t y2,
        uint8_t color = WHITE)
    {
        Arduboy2Base::fillTriangle(x0, y0, x1, y1, x2, y2, planeColor(current_plane, color));
    }
    
    template<uint8_t PLANE>
    static void fillTriangle(
        int16_t x0, int16_t y0,
        int16_t x1, int16_t y1,
        int16_t x2, int16_t y2,
        uint8_t color = WHITE)
    {
        Arduboy2Base::fillTriangle(x0, y0, x1, y1, x2, y2, planeColor<PLANE>(color));
    }
    
    static void fillRect(
        int16_t x, int16_t y,
        uint8_t w, uint8_t h,
        uint8_t color = WHITE)
    {
        Arduboy2Base::fillRect(x, y, w, h, planeColor(current_plane, color));
    }
    
    template<uint8_t PLANE>
    static void fillRect(
        int16_t x, int16_t y,
        uint8_t w, uint8_t h,
        uint8_t color = WHITE)
    {
        Arduboy2Base::fillRect(x, y, w, h, planeColor<PLANE>(color));
    }
    
    static void fillRoundRect(
        int16_t x, int16_t y,
        uint8_t w, uint8_t h,
        uint8_t r,
        uint8_t color = WHITE)
    {
        Arduboy2Base::fillRoundRect(x, y, w, h, r, planeColor(current_plane, color));
    }
    
    template<uint8_t PLANE>
    static void fillRoundRect(
        int16_t x, int16_t y,
        uint8_t w, uint8_t h,
        uint8_t r,
        uint8_t color = WHITE)
    {
        Arduboy2Base::fillRoundRect(x, y, w, h, r, planeColor<PLANE>(color));
    }
    
    static void fillScreen(
        uint8_t color = WHITE)
    {
        Arduboy2Base::fillScreen(planeColor(current_plane, color));
    }
    
    template<uint8_t PLANE>
    static void fillScreen(
        uint8_t color = WHITE)
    {
        Arduboy2Base::fillScreen(planeColor<PLANE>(color));
    }

    static bool needsUpdate(){
        if(update_flag == true){
            update_flag = false;
            return true;
        }
        return false;
    }

    
   static uint8_t currentPlane(){
      return current_plane;
    }

    
    static void waitForNextPlane(uint8_t clear = BLACK){
      current_plane++;
      if (current_plane >= num_planes(MODE)){
        current_plane = 0;
       }  
      doDisplay(clear);
    }
        
        
    ABG_NOT_SUPPORTED static void flipVertical();
    ABG_NOT_SUPPORTED static void paint8Pixels(uint8_t);
    ABG_NOT_SUPPORTED static void paintScreen(uint8_t const*);
    ABG_NOT_SUPPORTED static void paintScreen(uint8_t[], bool);
    //ABG_NOT_SUPPORTED static void setFrameDuration(uint8_t);
    //ABG_NOT_SUPPORTED static void setFrameRate(uint8_t);
    ABG_NOT_SUPPORTED static void display();
    ABG_NOT_SUPPORTED static void display(bool);
    //ABG_NOT_SUPPORTED static bool nextFrame();
    ABG_NOT_SUPPORTED static bool nextFrameDEV();


    // expose internal Arduboy2Core methods
    static void setCPUSpeed8MHz() { BASE::setCPUSpeed8MHz(); }
    static void bootSPI        () { BASE::bootSPI        (); }
    static void bootOLED       () { BASE::bootOLED       (); }
    static void bootPins       () { BASE::bootPins       (); }
    static void bootPowerSaving() { BASE::bootPowerSaving(); }
    
    // color conversion method
    static uint8_t color (uint8_t c) { return planeColor(current_plane, c); }
    static uint8_t colour(uint8_t c) { return planeColor(current_plane, c); }

    
protected:    
    ICACHE_RAM_ATTR static void doDisplay(uint8_t clear){
     static uint8_t keys;
     static bool keysPass=0;
     keysPass =! keysPass;
        
     if(keysPass){
        keys = myESPboy.getKeys();        
        if (keys&PAD_RGT || keys&PAD_LFT) {
           while(nbSPI_isBusy());
           noInterrupts();
           delay(50);
           keys = myESPboy.getKeys();
           if ((keys&0x40)/*PAD_LFT*/ && (keys&0x80)/*PAD_RGT*/) {arduboyYscaleFlag =! arduboyYscaleFlag;}
           else
           if (keys&0x40/*PAD_LFT*/) {paletteIndex--;}
           else
           if (keys&0x80/*PAD_RGT*/) {paletteIndex++;}
           
           if (paletteIndex<0) paletteIndex = MAX_PALETTES;
           if (paletteIndex>MAX_PALETTES) paletteIndex = 0;
           
           if(!arduboyYscaleFlag){
             myESPboy.tft.fillScreen(0);
             myESPboy.tft.setAddrWindow(0, VERT_OFFSET, WIDTH, HEIGHT);}
           else{
             myESPboy.tft.setAddrWindow(0, 0, WIDTH, HEIGHT*2);}
           
           for(uint8_t i=0; i<8; i++)
                         currentPalette[i] = SWPLH(
                                  myESPboy.tft.color24to16(
                                    pgm_read_dword(&
                                      palettesCollection[paletteIndex][pgm_read_byte(&(paletteDecodeTable[paletteDecodeTableIndex])[i])]
                                  )
                                    )
                                );
           while (myESPboy.getKeys()) delay(10);
           interrupts();
        }
    }
        
        if (current_plane == 0) {memcpy(plane0, b, 128*64/8);}
        if (current_plane == 1) {memcpy(plane1, b, 128*64/8);}

        if (current_plane == num_planes(MODE)-1){

        while (frameRateMillisPrev+frameRateDelayMs > millis());
        frameRateMillisPrev = millis();

//// START renderPlanesToLCD 
        union currentDBT {
         struct {
           uint8_t l;
           uint8_t h;} bit;
         uint16_t byte;
        };
          
        currentDBT currentDataByte1, currentDataByte2, currentDataByte3;
        uint16_t addr, currentDataAddr;
          
        for(uint8_t kPos = 0; kPos<4; kPos++){
         currentDataAddr = kPos * WIDTH * 2;
         if(MODE == ABG_Mode::L4_Triplane){
             for (uint8_t xPos = 0; xPos < WIDTH; xPos++) {
                currentDataByte1.bit.l = plane0[currentDataAddr];
                currentDataByte2.bit.l = plane1[currentDataAddr];
                currentDataByte3.bit.l = b[currentDataAddr];
                currentDataAddr += 128;
                currentDataByte1.bit.h = plane0[currentDataAddr];
                currentDataByte2.bit.h = plane1[currentDataAddr];
                currentDataByte3.bit.h = b[currentDataAddr];
                currentDataAddr -= 127;
                addr = xPos;
                for (uint8_t yPos = 0; yPos < 16; yPos++) {
                  oBuffer[addr] = (currentPalette[((currentDataByte3.byte & 0x01)<<1) | (currentDataByte2.byte & 0x01) | ((currentDataByte1.byte & 0x01)<<2)]);  
                  arduboyYscaleFlag ? addr+=WIDTH*2 : addr+=WIDTH;            
                  currentDataByte1.byte >>= 1;
                  currentDataByte2.byte >>= 1;
                  currentDataByte3.byte >>= 1;
                }
             }
          }
          
          else{
             for (uint8_t xPos = 0; xPos < WIDTH; xPos++) {
                currentDataByte1.bit.l = plane0[currentDataAddr];
                currentDataByte2.bit.l = plane1[currentDataAddr];
                currentDataAddr += 128;
                currentDataByte1.bit.h = plane0[currentDataAddr];         
                currentDataByte2.bit.h = plane1[currentDataAddr];
                currentDataAddr -= 127;
                addr = xPos;
                for (uint8_t yPos = 0; yPos < 16; yPos++) { 
                  oBuffer[addr] = (currentPalette[(currentDataByte2.byte & 0x01) | ((currentDataByte1.byte & 0x01)<<1)]);  
                  arduboyYscaleFlag ? addr+=WIDTH*2 : addr+=WIDTH; 
                  currentDataByte1.byte >>= 1;
                  currentDataByte2.byte >>= 1;
                }
             }           
          }

          addr = 0;
          if(arduboyYscaleFlag)
            for(uint8_t i=0; i<16; i++){
              memcpy(&oBuffer[addr+WIDTH], &oBuffer[addr], WIDTH*2);
              addr+=WIDTH*2;
            }
          
#ifndef USE_nbSPI
          arduboyYscaleFlag ? myESPboy.tft.pushColors(oBuffer, WIDTH*32, 0) : myESPboy.tft.pushColors(oBuffer, WIDTH*16, 0); 
#else          
          while(nbSPI_isBusy());
          arduboyYscaleFlag ? nbSPI_writeBytes((uint8_t*)oBuffer, WIDTH*64) : nbSPI_writeBytes((uint8_t*)oBuffer, WIDTH*32);         
          (oBuffer == oBuffer1) ? oBuffer = oBuffer2 : oBuffer = oBuffer1;
#endif            
         }
          
          (clear == BLACK) ? memset(b, 0, 128*64/8) : memset(b, 255, 128*64/8);
          
          update_every_n_count++;
          if(update_every_n_count > update_every_n){
             update_flag = true;
             update_every_n_count = 0;
          }
       }
// END OF renderPlanesToLCD 
    }
      
    
    // Plane                               0  1  2  index
    // ==================================================
    //
    // ABG_Mode::L4_Contrast   BLACK       .  .       0
    // ABG_Mode::L4_Contrast   DARK_GRAY   X  .       1 2 4
    // ABG_Mode::L4_Contrast   LIGHT_GRAY  .  X .     2 4 1
    // ABG_Mode::L4_Contrast   WHITE       X  X .     3 6 5
    //
    // ABG_Mode::L4_Triplane   BLACK       .  .  .    0 
    // ABG_Mode::L4_Triplane   DARK_GRAY   X  .  .    1 2 4
    // ABG_Mode::L4_Triplane   LIGHT_GRAY  X  X  .    3 6 5
    // ABG_Mode::L4_Triplane   WHITE       X  X  X .  7 
    //
    // ABG_Mode::L3            BLACK       .  .       0
    // ABG_Mode::L3            GRAY        X  .       1 2
    // ABG_Mode::L3            WHITE       X  X .     3

    template<uint8_t PLANE>
    static constexpr uint8_t planeColor(uint8_t color)
    {
        return
            MODE == ABG_Mode::L4_Contrast ? ((color & (PLANE + 1)) ? 1 : 0) :
            MODE == ABG_Mode::L4_Triplane ? ((color > PLANE) ? 1 : 0) :
            MODE == ABG_Mode::L3          ? ((color > PLANE) ? 1 : 0) :
            0;
    }

    static uint8_t planeColor(uint8_t plane, uint8_t color)
    {
        if(plane == 0)
            return planeColor<0>(color);
        else if(plane == 1 || MODE != ABG_Mode::L4_Triplane)
            return planeColor<1>(color);
        else
            return planeColor<2>(color);
    }

};

} // namespace abg_detail

template<
    ABG_Mode MODE = ABG_Mode::Default,
    uint8_t FLAGS = ABG_Flags::Default
>

using ArduboyGBase_Config = abg_detail::ArduboyG_Common<
    Arduboy2Base, MODE, FLAGS>;

template<
    ABG_Mode MODE = ABG_Mode::Default,
    uint8_t FLAGS = ABG_Flags::Default
>

struct ArduboyG_Config : public abg_detail::ArduboyG_Common<
    Arduboy2, MODE, FLAGS>
{
    using BASE = abg_detail::ArduboyG_Common<Arduboy2, MODE, FLAGS>;
    
    static void startGray()
    {
        ArduboyGBase_Config<MODE, FLAGS>::startGray();
        //Arduboy2::setTextColor(WHITE); // WHITE is 3 not 1
    }
    
    static void startGrey() { startGray(); }
    
    void drawChar(
        int16_t x, int16_t y,
        uint8_t c,
        uint8_t color, uint8_t bg,
        uint8_t size)
    {        
        using A = Arduboy2;
        color = BASE::planeColor(abg_detail::current_plane, color);
        bg    = BASE::planeColor(abg_detail::current_plane, bg);
        
        uint8_t fullCharacterWidth = A::getCharacterWidth() + A::getCharacterSpacing();
        if(color == bg)
            BASE::fillRect(x, y, size * fullCharacterWidth, size * A::getCharacterHeight(), bg);
        else
            A::drawChar(x, y, c, color, bg, size);
    }
    
    // duplicate from Arduboy2 code to use overridden drawChar
    size_t write(uint8_t c) override
    {        
        using A = Arduboy2;
    
        if ((c == '\r') && !A::textRaw)
        {
            return 1;
        }

        if (((c == '\n') && !A::textRaw) ||
            (A::textWrap && (A::cursor_x > (WIDTH - (A::characterWidth * A::textSize)))))
        {
            A::cursor_x = 0;
            A::cursor_y += A::fullCharacterHeight * A::textSize;
        }

        if ((c != '\n') || A::textRaw)
        {
            uint8_t fullCharacterWidth = A::getCharacterWidth() + A::getCharacterSpacing();
            drawChar(A::cursor_x, A::cursor_y, c, A::textColor, A::textBackground, A::textSize);
            A::cursor_x += fullCharacterWidth * A::textSize;
        }

        return 1;
    }
};
    
using ArduboyGBase = ArduboyGBase_Config<>;
using ArduboyG     = ArduboyG_Config<>;

#ifdef ABG_IMPLEMENTATION
namespace abg_detail
{
bool  update_flag;
uint8_t  current_plane;
uint8_t  update_every_n = ABG_UPDATE_EVERY_N_DEFAULT;
uint8_t  update_every_n_count = 0;
}
#endif
