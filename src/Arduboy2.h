/**
 * @file Arduboy2.h
 * \brief
 * The Arduboy2Base and Arduboy2 classes and support objects and definitions.
 */

#ifndef ARDUBOY2_H
#define ARDUBOY2_H

#include "SPI.h"
#include "TFT_eSPI.h"

#include <Arduino.h>
#include <ESP_EEPROM.h>
#include "Arduboy2Core.h"
#include "Arduboy2Beep.h"
#include "Sprites.h"
#include "SpritesB.h"
#include <Print.h>
#include <limits.h>
#include "glcdfont.c"

#define PAD_LEFT        0x01
#define PAD_UP          0x02
#define PAD_DOWN        0x04
#define PAD_RIGHT       0x08
#define PAD_ACT         0x10
#define PAD_ESC         0x20
#define PAD_LFT         0x40
#define PAD_RGT         0x80
#define PAD_ANY         0xff

/** \brief
 * Library version
 *
 * \details
 * For a version number in the form of x.y.z the value of the define will be
 * ((x * 10000) + (y * 100) + (z)) as a decimal number.
 * So, it will read as xxxyyzz, with no leading zeros on x.
 *
 * A user program can test this value to conditionally compile based on the
 * library version. For example:
 *
 * \code{.cpp}
 * // If the library is version 2.1.0 or higher
 * #if ARDUBOY_LIB_VER >= 20100
 *   // ... code that make use of a new feature added to V2.1.0
 * #endif
 * \endcode
 */
#define ARDUBOY_LIB_VER 50100
#define ARDUBOY_ID 0xFAEF
#define EEPROM_STORAGE_SPACE_START_SETTINGS 1
#define EEPROM_STORAGE_SPACE_START 20


// EEPROM settings
struct ArduboySettings{
 uint32_t arduboyID = ARDUBOY_ID;
 uint8_t arduboyBackground = 4;
 uint8_t arduboyForeground = 14;
 bool arduboyAudioOnOff = 1; 
 bool arboyLogo = 0;
 bool arduboyLeds = 0;
 bool arduboyYscale = 0;
};


// eeprom settings above are neded for audio
#include "Arduboy2Audio.h"

// If defined, it is safe to draw outside of the screen boundaries.
// Pixels that would exceed the display limits will be ignored.
#define PIXEL_SAFE_MODE

// pixel colors
#define BLACK 0  /**< Color value for an unlit pixel for draw functions. */
#define WHITE 1  /**< Color value for a lit pixel for draw functions. */
/** \brief
 * Color value to indicate pixels are to be inverted.
 *
 * \details
 * BLACK pixels will become WHITE and WHITE will become BLACK.
 *
 * \note
 * Only function Arduboy2Base::drawBitmap() currently supports this value.
 */
#define INVERT 2

#define CLEAR_BUFFER true /**< Value to be passed to `display()` to clear the screen buffer. */


/** \brief
 * A rectangle object for collision functions.
 *
 * \details
 * The X and Y coordinates specify the top left corner of a rectangle with the
 * given width and height.
 *
 * \see Arduboy2Base::collide(Point, Rect) Arduboy2Base::collide(Rect, Rect)
 */
struct Rect{
  int16_t x;      /**< The X coordinate of the top left corner */
  int16_t y;      /**< The Y coordinate of the top left corner */
  uint8_t width;  /**< The width of the rectangle */
  uint8_t height; /**< The height of the rectangle */

  /** \brief
   * The default constructor
   */
  Rect() = default;

  /** \brief
   * The fully initializing constructor
   *
   * \param x The X coordinate of the top left corner. Copied to variable `x`.
   * \param y The Y coordinate of the top left corner. Copied to variable `y`.
   * \param width The width of the rectangle. Copied to variable `width`.
   * \param height The height of the rectangle. Copied to variable `height`.
   */
  constexpr Rect(int16_t x, int16_t y, uint8_t width, uint8_t height)
    : x(x), y(y), width(width), height(height)
  {
  }
};

/** \brief
 * An object to define a single point for collision functions.
 *
 * \details
 * The location of the point is given by X and Y coordinates.
 *
 * \see Arduboy2Base::collide(Point, Rect)
 */
struct Point
{
  int16_t x; /**< The X coordinate of the point */
  int16_t y; /**< The Y coordinate of the point */
};

//==================================
//========== Arduboy2Base ==========
//==================================

/** \brief
 * The main functions provided for writing sketches for the Arduboy,
 * _minus_ text output.
 *
 * \details
 * This class in inherited by Arduboy2, so if text output functions are
 * required Arduboy2 should be used instead.
 *
 * \note
 * \parblock
 * An Arduboy2Audio class object named `audio` will be created by the
 * Arduboy2Base class, so there is no need for a sketch itself to create an
 * Arduboy2Audio object. Arduboy2Audio functions can be called using the
 * Arduboy2 or Arduboy2Base `audio` object.
 *
 * Example:
 *
 * \code{.cpp}
 * #include <Arduboy2.h>
 *
 * Arduboy2 arduboy;
 *
 * // Arduboy2Audio functions can be called as follows:
 *   arduboy.audio.on();
 *   arduboy.audio.off();
 * \endcode
 * \endparblock
 *
 * \note
 * \parblock
 * A friend class named _Arduboy2Ex_ is declared by this class. The intention
 * is to allow a sketch to create an _Arduboy2Ex_ class which would have access
 * to the private and protected members of the Arduboy2Base class. It is hoped
 * that this may eliminate the need to create an entire local copy of the
 * library, in order to extend the functionality, in most circumstances.
 * \endparblock
 *
 * \see Arduboy2
 */
class Arduboy2Base : public Arduboy2Core
{
 friend class Arduboy2Ex;

 public:
  Arduboy2Base();

  /** \brief
   * An object created to provide audio control functions within this class.
   *
   * \details
   * This object is created to eliminate the need for a sketch to create an
   * Arduboy2Audio class object itself.
   *
   * \see Arduboy2Audio
   */
  static Arduboy2Audio audio;
  BeepPin1 tunes;
  
  /** \brief
   * Initialize the hardware, display the boot logo, provide boot utilities, etc.
   *
   * \details
   * This function should be called once near the start of the sketch,
   * usually in `setup()`, before using any other functions in this class.
   * It initializes the display, displays the boot logo, provides "flashlight"
   * and system control features and initializes audio control.
   *
   * \note
   * To free up some code space for use by the sketch, `boot()` can be used
   * instead of `begin()` to allow the elimination of some of the things that
   * aren't really required, such as displaying the boot logo.
   *
   * \see boot()
   */
  static void start();
  static void begin();

  /** \brief
   * Turn the RGB LED and display fully on to act as a small flashlight/torch.
   *
   * \details
   * Checks if the UP button is pressed and if so turns the RGB LED and all
   * display pixels fully on. If the UP button is detected, this function
   * does not exit. The Arduboy must be restarted after flashlight mode is used.
   *
   * This function is called by `begin()` and can be called by a sketch
   * after `boot()`.
   *
   * \note
   * \parblock
   * This function also contains code to address a problem with uploading a new
   * sketch, for sketches that interfere with the bootloader "magic number".
   * This problem occurs with certain sketches that use large amounts of RAM.
   * Being in flashlight mode when uploading a new sketch can fix this problem.
   *
   * Therefore, for sketches that potentially could cause this problem, and use
   * `boot()` instead of `begin()`, it is recommended that a call to
   * `flashlight()` be included after calling `boot()`. If program space is
   * limited, `safeMode()` can be used instead of `flashlight()`.
   * \endparblock
   *
   * \see begin() boot() safeMode()
   */
 static void flashlight();

  /** \brief
   * Handle buttons held on startup for system control.
   *
   * \details
   * This function is called by `begin()` and can be called by a sketch
   * after `boot()`.
   *
   * Hold the B button when booting to enter system control mode.
   * The B button must be held continuously to remain in this mode.
   * Then, pressing other buttons will perform system control functions:
   *
   * - UP: Set "sound enabled" in EEPROM
   * - DOWN: Set "sound disabled" (mute) in EEPROM
   *
   * \see begin() boot()
   */
  static void systemButtons();

  /** \brief
   * Wait until all buttons have been released.
   *
   * \details
   * This function is called by `begin()` and can be called by a sketch
   * after `boot()`.
   *
   * It won't return unless no buttons are being pressed. A short delay is
   * performed each time before testing the state of the buttons to do a
   * simple button debounce.
   *
   * This function is called at the end of `begin()` to make sure no buttons
   * used to perform system start up actions are still being pressed, to
   * prevent them from erroneously being detected by the sketch code itself.
   *
   * \see begin() boot()
   */
  static void waitNoButtons();

  /** \brief
   * Clear the display buffer.
   *
   * \details
   * The entire contents of the screen buffer are cleared to BLACK.
   *
   * \see display(bool)
   */
   
  static void clearDisplay();
   
  static void clear();

  /** \brief
   * Copy the contents of the display buffer to the display.
   *
   * \details
   * The contents of the display buffer in RAM are copied to the display and
   * will appear on the screen.
   *
   * \see display(bool)
   */
  static void display();

  /** \brief
   * Copy the contents of the display buffer to the display. The display buffer
   * can optionally be cleared.
   *
   * \param clear If `true` the display buffer will be cleared to zero.
   * The defined value `CLEAR_BUFFER` should be used instead of `true` to make
   * it more meaningful.
   *
   * \details
   * Operation is the same as calling `display()` without parameters except
   * additionally the display buffer will be cleared if the parameter evaluates
   * to `true`. (The defined value `CLEAR_BUFFER` can be used for this)
   *
   * Using `display(CLEAR_BUFFER)` is faster and produces less code than
   * calling `display()` followed by `clear()`.
   *
   * \see display() clear()
   */
  static void display(bool clear);

  /** \brief
   * Set a single pixel in the display buffer to the specified color.
   *
   * \param x The X coordinate of the pixel.
   * \param y The Y coordinate of the pixel.
   * \param color The color of the pixel (optional; defaults to WHITE).
   *
   * \details
   * The single pixel specified location in the display buffer is set to the
   * specified color. The values WHITE or BLACK can be used for the color.
   * If the `color` parameter isn't included, the pixel will be set to WHITE.
   */
  static void drawPixel(int16_t x, int16_t y, uint8_t color = WHITE);

  /** \brief
   * Returns the state of the given pixel in the screen buffer.
   *
   * \param x The X coordinate of the pixel.
   * \param y The Y coordinate of the pixel.
   *
   * \return WHITE if the pixel is on or BLACK if the pixel is off.
   */
  static uint8_t getPixel(uint8_t x, uint8_t y);

  /** \brief
   * Draw a circle of a given radius.
   *
   * \param x0 The X coordinate of the circle's center.
   * \param y0 The Y coordinate of the circle's center.
   * \param r The radius of the circle in pixels.
   * \param color The circle's color (optional; defaults to WHITE).
   */
  static void drawCircle(int16_t x0, int16_t y0, uint8_t r, uint8_t color = WHITE);

  // Draw one or more "corners" of a circle.
  // (Not officially part of the API)
  static void drawCircleHelper(int16_t x0, int16_t y0, uint8_t r, uint8_t corners, uint8_t color = WHITE);

  /** \brief
   * Draw a filled-in circle of a given radius.
   *
   * \param x0 The X coordinate of the circle's center.
   * \param y0 The Y coordinate of the circle's center.
   * \param r The radius of the circle in pixels.
   * \param color The circle's color (optional; defaults to WHITE).
   */
  static void fillCircle(int16_t x0, int16_t y0, uint8_t r, uint8_t color = WHITE);

  // Draw one or both vertical halves of a filled-in circle or
  // rounded rectangle edge.
  // (Not officially part of the API)
  static void fillCircleHelper(int16_t x0, int16_t y0, uint8_t r, uint8_t sides, int16_t delta, uint8_t color = WHITE);

  /** \brief
   * Draw a line between two specified points.
   *
   * \param x0,x1 The X coordinates of the line ends.
   * \param y0,y1 The Y coordinates of the line ends.
   * \param color The line's color (optional; defaults to WHITE).
   *
   * \details
   * Draw a line from the start point to the end point using
   * Bresenham's algorithm.
   * The start and end points can be at any location with respect to the other.
   */
  static void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color = WHITE);

  /** \brief
   * Draw a rectangle of a specified width and height.
   *
   * \param x The X coordinate of the upper left corner.
   * \param y The Y coordinate of the upper left corner.
   * \param w The width of the rectangle.
   * \param h The height of the rectangle.
   * \param color The color of the pixel (optional; defaults to WHITE).
   */
  static void drawRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color = WHITE);

  /** \brief
   * Draw a vertical line.
   *
   * \param x The X coordinate of the upper start point.
   * \param y The Y coordinate of the upper start point.
   * \param h The height of the line.
   * \param color The color of the line (optional; defaults to WHITE).
   */
  static void drawFastVLine(int16_t x, int16_t y, uint8_t h, uint8_t color = WHITE);

  /** \brief
   * Draw a horizontal line.
   *
   * \param x The X coordinate of the left start point.
   * \param y The Y coordinate of the left start point.
   * \param w The width of the line.
   * \param color The color of the line (optional; defaults to WHITE).
   */
  static void drawFastHLine(int16_t x, int16_t y, uint8_t w, uint8_t color = WHITE);

  /** \brief
   * Draw a filled-in rectangle of a specified width and height.
   *
   * \param x The X coordinate of the upper left corner.
   * \param y The Y coordinate of the upper left corner.
   * \param w The width of the rectangle.
   * \param h The height of the rectangle.
   * \param color The color of the pixel (optional; defaults to WHITE).
   */
  static void fillRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color = WHITE);

  /** \brief
   * Fill the screen buffer with the specified color.
   *
   * \param color The fill color (optional; defaults to WHITE).
   */
  static void fillScreen(uint8_t color = WHITE);

  /** \brief
   * Draw a rectangle with rounded corners.
   *
   * \param x The X coordinate of the left edge.
   * \param y The Y coordinate of the top edge.
   * \param w The width of the rectangle.
   * \param h The height of the rectangle.
   * \param r The radius of the semicircles forming the corners.
   * \param color The color of the rectangle (optional; defaults to WHITE).
   */
  static void drawRoundRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t color = WHITE);

  /** \brief
   * Draw a filled-in rectangle with rounded corners.
   *
   * \param x The X coordinate of the left edge.
   * \param y The Y coordinate of the top edge.
   * \param w The width of the rectangle.
   * \param h The height of the rectangle.
   * \param r The radius of the semicircles forming the corners.
   * \param color The color of the rectangle (optional; defaults to WHITE).
   */
  static void fillRoundRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t color = WHITE);

  /** \brief
   * Draw a triangle given the coordinates of each corner.
   *
   * \param x0,x1,x2 The X coordinates of the corners.
   * \param y0,y1,y2 The Y coordinates of the corners.
   * \param color The triangle's color (optional; defaults to WHITE).
   *
   * \details
   * A triangle is drawn by specifying each of the three corner locations.
   * The corners can be at any position with respect to the others.
   */
 static void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color = WHITE);

  /** \brief
   * Draw a filled-in triangle given the coordinates of each corner.
   *
   * \param x0,x1,x2 The X coordinates of the corners.
   * \param y0,y1,y2 The Y coordinates of the corners.
   * \param color The triangle's color (optional; defaults to WHITE).
   *
   * \details
   * A triangle is drawn by specifying each of the three corner locations.
   * The corners can be at any position with respect to the others.
   */
 static void fillTriangle (int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color = WHITE);

  /** \brief
   * Draw a bitmap from an array in program memory.
   *
   * \param x The X coordinate of the top left pixel affected by the bitmap.
   * \param y The Y coordinate of the top left pixel affected by the bitmap.
   * \param bitmap A pointer to the bitmap array in program memory.
   * \param w The width of the bitmap in pixels.
   * \param h The height of the bitmap in pixels.
   * \param color The color of pixels for bits set to 1 in the bitmap.
   *              If the value is INVERT, bits set to 1 will invert the
   *              corresponding pixel. (optional; defaults to WHITE).
   *
   * \details
   * Bits set to 1 in the provided bitmap array will have their corresponding
   * pixel set to the specified color. For bits set to 0 in the array, the
   * corresponding pixel will be left unchanged.
   *
   * Each byte in the array specifies a vertical column of 8 pixels, with the
   * least significant bit at the top.
   *
   * The array must be located in program memory by using the PROGMEM modifier.
   */
  static void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color = WHITE);

  /** \brief
   * Draw a bitmap from a horizontally oriented array in program memory.
   *
   * \param x The X coordinate of the top left pixel affected by the bitmap.
   * \param y The Y coordinate of the top left pixel affected by the bitmap.
   * \param bitmap A pointer to the bitmap array in program memory.
   * \param w The width of the bitmap in pixels.
   * \param h The height of the bitmap in pixels.
   * \param color The color of pixels for bits set to 1 in the bitmap.
   *              (optional; defaults to WHITE).
   *
   * \details
   * Bits set to 1 in the provided bitmap array will have their corresponding
   * pixel set to the specified color. For bits set to 0 in the array, the
   * corresponding pixel will be left unchanged.
   *
   * Each byte in the array specifies a horizontal row of 8 pixels, with the
   * most significant bit at the left end of the row.
   *
   * The array must be located in program memory by using the PROGMEM modifier.
   *
   * \note
   * This function requires a lot of additional CPU power and will draw images
   * slower than `drawBitmap()`, which uses bitmaps that are stored in a format
   * that allows them to be directly written to the screen. It is recommended
   * you use `drawBitmap()` when possible.
   */
  static void drawSlowXYBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color = WHITE);

  /** \brief
   * Draw a bitmap from an array of compressed data.
   *
   * \param sx The X coordinate of the top left pixel affected by the bitmap.
   * \param sy The Y coordinate of the top left pixel affected by the bitmap.
   * \param bitmap A pointer to the compressed bitmap array in program memory.
   * \param color The color of pixels for bits set to 1 in the bitmap.
   *              (optional; defaults to WHITE).
   *
   * \details
   * Draw a bitmap starting at the given coordinates from an array that has
   * been compressed using an algorthm implemented by Team A.R.G.
   * For more information see:
   * https://github.com/TEAMarg/drawCompressed
   * https://github.com/TEAMarg/Cabi
   *
   * Bits set to 1 in the provided bitmap array will have their corresponding
   * pixel set to the specified color. For bits set to 0 in the array, the
   * corresponding pixel will be left unchanged.
   *
   * The array must be located in program memory by using the PROGMEM modifier.
   */
  static void drawCompressed(int16_t sx, int16_t sy, const uint8_t *bitmap, uint8_t color = WHITE);

  /** \brief
   * Get a pointer to the display buffer in RAM.
   *
   * \return A pointer to the display buffer array in RAM.
   *
   * \details
   * The location of the display buffer in RAM, which is displayed using
   * `display()`, can be gotten using this function. The buffer can then be
   *  read and directly manipulated.
   *
   * \note
   * The display buffer array, `sBuffer`, is public. A sketch can access it
   * directly. Doing so may be more efficient than accessing it via the
   * pointer returned by `getBuffer()`.
   *
   * \see sBuffer
   */
  static uint8_t* getBuffer();

  /** \brief
   * Seed the random number generator with a random value.
   *
   * \details
   * The Arduino random number generator is seeded with a random value
   * derived from entropy from an ADC reading of a floating pin combined with
   * the microseconds since boot.
   *
   * This method is still most effective when called after a semi-random time,
   * such as after a user hits a button to start a game or other semi-random
   * event.
   */
  static void initRandomSeed();

  // Swap the values of two int16_t variables passed by reference.
  static void swap(int16_t& a, int16_t& b);

  /** \brief
   * Set the frame rate used by the frame control functions.
   *
   * \param rate The desired frame rate in frames per second.
   *
   * \details
   * Set the frame rate, in frames per second, used by `nextFrame()` to update
   * frames at a given rate. If this function or `setFrameDuration()`
   * isn't used, the default rate will be 60 (actually 62.5, see note below).
   *
   * Normally, the frame rate would be set to the desired value once, at the
   * start of the game, but it can be changed at any time to alter the frame
   * update rate.
   *
   * \note
   * \parblock
   * The given rate is internally converted to a frame duration in milliseconds,
   * rounded down to the nearest integer. Therefore, the actual rate will be
   * equal to or higher than the rate given.

   * For example, 60 FPS would be 16.67ms per frame. This will be rounded down
   * to 16ms, giving an actual frame rate of 62.5 FPS.
   * \endparblock
   *
   * \see nextFrame() setFrameDuration()
   */
  void setFrameRate(uint16_t rate);

  /** \brief
   * Set the frame rate, used by the frame control functions, by giving
   * the duration of each frame.
   *
   * \param duration The desired duration of each frame in milliseconds.
   *
   * \details
   * Set the frame rate by specifying the duration of each frame in
   * milliseconds. This is used by `nextFrame()` to update frames at a
   * given rate. If this function or `setFrameRate()` isn't used,
   * the default will be 16ms per frame.
   *
   * Normally, the frame rate would be set to the desired value once, at the
   * start of the game, but it can be changed at any time to alter the frame
   * update rate.
   *
   * \see nextFrame() setFrameRate()
   */
  void setFrameDuration(uint8_t duration);

  /** \brief
   * Indicate that it's time to render the next frame.
   *
   * \return `true` if it's time for the next frame.
   *
   * \details
   * When this function returns `true`, the amount of time has elapsed to
   * display the next frame, as specified by `setFrameRate()`.
   *
   * This function will normally be called at the start of the rendering loop
   * which would wait for `true` to be returned before rendering and
   * displaying the next frame.
   *
   * example:
   * \code{.cpp}
   * void loop() {
   *   if (!arduboy.nextFrame()) {
   *     return; // go back to the start of the loop
   *   }
   *   // render and display the next frame
   * }
   * \endcode
   *
   * \see setFrameRate() setFrameDuration() nextFrameDEV()
   */
  bool nextFrame();

  /** \brief
   * Indicate that it's time to render the next frame, and visually indicate
   * if the code is running slower than the desired frame rate.
   * **FOR USE DURING DEVELOPMENT**
   *
   * \return `true` if it's time for the next frame.
   *
   * \details
   * This function is intended to be used in place of `nextFrame()` during the
   * development of a sketch. It does the same thing as `nextFrame()` but
   * additionally will light the yellow TX LED (at the bottom, to the left
   * of the USB connector) whenever a frame takes longer to generate than the
   * time allotted per frame, as determined by the `setFrameRate()` function.
   *
   * Therefore, whenever the TX LED comes on (while not communicating over
   * USB), it indicates that the sketch is running slower than the desired
   * rate set by `setFrameRate()`. In this case the developer may wish to set
   * a slower frame rate, or reduce or optimize the code for such frames.
   *
   * \note
   * Once a sketch is ready for release, it would be expected that
   * `nextFrameDEV()` calls be restored to `nextFrame()`.
   *
   * \see nextFrame() cpuLoad() setFrameRate()
   */
  bool nextFrameDEV();

  /** \brief
   * Indicate if the specified number of frames has elapsed.
   *
   * \param frames The desired number of elapsed frames.
   *
   * \return `true` if the specified number of frames has elapsed.
   *
   * \details
   * This function should be called with the same value each time for a given
   * event. It will return `true` if the given number of frames has elapsed
   * since the previous frame in which it returned `true`.
   *
   * For example, if you wanted to fire a shot every 5 frames while the A button
   * is being held down:
   *
   * \code{.cpp}
   * if (arduboy.everyXFrames(5)) {
   *   if arduboy.pressed(A_BUTTON) {
   *     fireShot();
   *   }
   * }
   * \endcode
   *
   * \see setFrameRate() nextFrame()
   */
  bool everyXFrames(uint8_t frames);

  /** \brief
   * Return the load on the CPU as a percentage.
   *
   * \return The load on the CPU as a percentage of the total frame time.
   *
   * \details
   * The returned value gives the time spent processing a frame as a percentage
   * the total time allotted for a frame, as determined by the frame rate.
   *
   * This function normally wouldn't be used in the final program. It is
   * intended for use during program development as an aid in helping with
   * frame timing.
   *
   * \note
   * The percentage returned can be higher than 100 if more time is spent
   * processing a frame than the time allotted per frame. This would indicate
   * that the frame rate should be made slower or the frame processing code
   * should be optimized to run faster.
   *
   * \see setFrameRate() nextFrame()
   */
  int cpuLoad();

  /** \brief
   * Test if the specified buttons are pressed.
   *
   * \param buttons A bit mask indicating which buttons to test.
   * (Can be a single button)
   *
   * \return `true` if *all* buttons in the provided mask are currently pressed.
   *
   * \details
   * Read the state of the buttons and return `true` if all the buttons in the
   * specified mask are being pressed.
   *
   * Example: `if (pressed(LEFT_BUTTON + A_BUTTON))`
   *
   * \note
   * This function does not perform any button debouncing.
   */
  static bool pressed(uint8_t buttons);

/** \brief
   * Test if any of the specified buttons are pressed.
   *
   * \param buttons A bit mask indicating which buttons to test.
   * (Can be a single button)
   *
   * \return `true` if *one or more* of the buttons in the provided mask are
   * currently pressed.
   *
   * \details
   * Read the state of the buttons and return `true` if one or more of the
   * buttons in the specified mask are being pressed.
   *
   * Example: `if (anyPressed(RIGHT_BUTTON | LEFT_BUTTON))`
   *
   * \note
   * This function does not perform any button debouncing.
   *
   * \see pressed() notPressed()
   */
  static bool anyPressed(uint8_t buttons);


  /** \brief
   * Test if the specified buttons are not pressed.
   *
   * \param buttons A bit mask indicating which buttons to test.
   * (Can be a single button)
   *
   * \return `true` if *all* buttons in the provided mask are currently
   * released.
   *
   * \details
   * Read the state of the buttons and return `true` if all the buttons in the
   * specified mask are currently released.
   *
   * Example: `if (notPressed(UP_BUTTON))`
   *
   * \note
   * This function does not perform any button debouncing.
   */
  static bool notPressed(uint8_t buttons);

  /** \brief
   * Poll the buttons and track their state over time.
   *
   * \details
   * Read and save the current state of the buttons and also keep track of the
   * button state when this function was previouly called. These states are
   * used by the `justPressed()` and `justReleased()` functions to determine
   * if a button has changed state between now and the previous call to
   * `pollButtons()`.
   *
   * This function should be called once at the start of each new frame.
   *
   * The `justPressed()` and `justReleased()` functions rely on this function.
   *
   * example:
   * \code{.cpp}
   * void loop() {
   *   if (!arduboy.nextFrame()) {
   *     return;
   *   }
   *   arduboy.pollButtons();
   *
   *   // use justPressed() as necessary to determine if a button was just pressed
   * \endcode
   *
   * \note
   * As long as the elapsed time between calls to this function is long
   * enough, buttons will be naturally debounced. Calling it once per frame at
   * a frame rate of 60 or lower (or possibly somewhat higher), should be
   * sufficient.
   *
   * \see justPressed() justReleased()
   */
  void pollButtons();

  /** \brief
   * Check if a button has just been pressed.
   *
   * \param button The button to test for. Only one button should be specified.
   *
   * \return `true` if the specified button has just been pressed.
   *
   * \details
   * Return `true` if the given button was pressed between the latest
   * call to `pollButtons()` and previous call to `pollButtons()`.
   * If the button has been held down over multiple polls, this function will
   * return `false`.
   *
   * There is no need to check for the release of the button since it must have
   * been released for this function to return `true` when pressed again.
   *
   * This function should only be used to test a single button.
   *
   * \see pollButtons() justReleased()
   */
 bool justPressed(uint8_t button);

  /** \brief
   * Check if a button has just been released.
   *
   * \param button The button to test for. Only one button should be specified.
   *
   * \return `true` if the specified button has just been released.
   *
   * \details
   * Return `true` if the given button, having previously been pressed,
   * was released between the latest call to `pollButtons()` and previous call
   * to `pollButtons()`. If the button has remained released over multiple
   * polls, this function will return `false`.
   *
   * There is no need to check for the button having been pressed since it must
   * have been previously pressed for this function to return `true` upon
   * release.
   *
   * This function should only be used to test a single button.
   *
   * \note
   * There aren't many cases where this function would be needed. Wanting to
   * know if a button has been released, without knowing when it was pressed,
   * is uncommon.
   *
   * \see pollButtons() justPressed()
   */
bool justReleased(uint8_t button);

  /** \brief
   * Test if a point falls within a rectangle.
   *
   * \param point A structure describing the location of the point.
   * \param rect A structure describing the location and size of the rectangle.
   *
   * \return `true` if the specified point is within the specified rectangle.
   *
   * \details
   * This function is intended to detemine if an object, whose boundaries are
   * are defined by the given rectangle, is in contact with the given point.
   *
   * \see Point Rect
   */
  static bool collide(Point point, Rect rect);

  /** \brief
   * Test if a rectangle is intersecting with another rectangle.
   *
   * \param rect1,rect2 Structures describing the size and locations of the
   * rectangles.
   *
   * \return `true` if the first rectangle is intersecting the second.
   *
   * \details
   * This function is intended to detemine if an object, whose boundaries are
   * are defined by the given rectangle, is in contact with another rectangular
   * object.
   *
   * \see Rect
   */
  static bool collide(Rect rect1, Rect rect2);

  /** \brief
   * Read the unit ID from system EEPROM.
   *
   * \return The value of the unit ID stored in system EEPROM.
   *
   * \details
   * This function reads the unit ID that has been set in system EEPROM.
   * The ID can be any value. It is intended to allow different units to be
   * uniquely identified.
   *
   * \see writeUnitID() readUnitName()
   */
  static uint16_t readUnitID();

  /** \brief
   * Write a unit ID to system EEPROM.
   *
   * \param id The value of the unit ID to be stored in system EEPROM.
   *
   * \details
   * This function writes a unit ID to a reserved location in system EEPROM.
   * The ID can be any value. It is intended to allow different units to be
   * uniquely identified.
   *
   * \see readUnitID() writeUnitName()
   */
  static void writeUnitID(uint16_t id);

  /** \brief
   * Read the unit name from system EEPROM.
   *
   * \param name A pointer to a string array variable where the unit name will
   * be placed. The string will be up to 6 characters and terminated with a
   * null (0x00) character, so the provided array must be at least 7 bytes long.
   *
   * \return The length of the string (0-6).
   *
   * \details
   * This function reads the unit name that has been set in system EEPROM. The
   * name is in ASCII and can contain any values except 0xFF and the
   * null (0x00) terminator value.
   *
   * The name can be used for any purpose. It could identify the owner or
   * give the unit itself a nickname. A sketch could use it to automatically
   * fill in a name or initials in a high score table, or display it as the
   * "player" when the opponent is the computer.
   *
   * \note
   * Sketches can use the defined value `ARDUBOY_UNIT_NAME_LEN` instead of
   * hard coding a 6 when working with the unit name. For example, to allocate
   * a buffer and read the unit name into it:
   * \code{.cpp}
   * // Buffer for maximum name length plus the terminator
   * char unitName[ARDUBOY_UNIT_NAME_LEN + 1];
   *
   * // The actual name length
   * byte unitNameLength;
   *
   * unitNameLength = arduboy.readUnitName(unitName);
   * \endcode
   *
   * \see writeUnitName() readUnitID() Arduboy2::bootLogoExtra()
   */
  static uint8_t readUnitName(char* name);

  /** \brief
   * Write a unit name to system EEPROM.
   *
   * \param name A pointer to a string array variable containing the unit name
   * to be saved. The string can be up to 6 characters and must be terminated
   * with a null (0x00) character. It can contain any values except 0xFF.
   *
   * \details
   * This function writes a unit name to a reserved area in system EEPROM.
   * The name is in ASCII and can contain any values except 0xFF and the
   * null (0x00) terminator value. The newline character (LF, \\n, 0x0A) and
   * carriage return character (CR, \\r, 0x0D) should also be avoided.
   *
   * The name can be used for any purpose. It could identify the owner or
   * give the unit itself a nickname. A sketch could use it to automatically
   * fill in a name or initials in a high score table, or display it as the
   * "player" when the opponent is the computer.
   *
   * \note
   * Sketches can use the defined value `ARDUBOY_UNIT_NAME_LEN` instead of
   * hard coding a 6 when working with the unit name.
   *
   * \see readUnitName() writeUnitID() Arduboy2::bootLogoExtra()
   */
  static void writeUnitName(char* name);

  /** \brief
   * Read the "Show Boot Logo" flag in system EEPROM.
   *
   * \return `true` if the flag is set to indicate that the boot logo sequence
   * should be displayed. `false` if the flag is set to not display the
   * boot logo sequence.
   *
   * \details
   * The "Show Boot Logo" flag is used to determine whether the system
   * boot logo sequence is to be displayed when the system boots up.
   * This function returns the value of this flag.
   *
   * \see writeShowBootLogoFlag() bootLogo()
   */
  static bool readShowBootLogoFlag();

  /** \brief
   * Write the "Show Boot Logo" flag in system EEPROM.
   *
   * \param val If `true` the flag is set to indicate that the boot logo
   * sequence should be displayed. If `false` the flag is set to not display
   * the boot logo sequence.
   *
   * \details
   * The "Show Boot Logo" flag is used to determine whether the system
   * boot logo sequence is to be displayed when the system boots up.
   * This function allows the flag to be saved with the desired value.
   *
   * \see readShowBootLogoFlag() bootLogo()
   */
  static void writeShowBootLogoFlag(bool val);

  /** \brief
   * Read the "Show Unit Name" flag in system EEPROM.
   *
   * \return `true` if the flag is set to indicate that the unit name should
   * be displayed. `false` if the flag is set to not display the unit name.
   *
   * \details
   * The "Show Unit Name" flag is used to determine whether the system
   * unit name is to be displayed at the end of the boot logo sequence.
   * This function returns the value of this flag.
   *
   * \see writeShowUnitNameFlag() writeUnitName() readUnitName()
   * Arduboy2::bootLogoExtra()
   */
  static bool readShowUnitNameFlag();

  /** \brief
   * Write the "Show Unit Name" flag in system EEPROM.
   *
   * \param val If `true` the flag is set to indicate that the unit name should
   * be displayed. If `false` the flag is set to not display the unit name.
   *
   * \details
   * The "Show Unit Name" flag is used to determine whether the system
   * unit name is to be displayed at the end of the boot logo sequence.
   * This function allows the flag to be saved with the desired value.
   *
   * \see readShowUnitNameFlag() writeUnitName() readUnitName()
   * Arduboy2::bootLogoExtra()
   */
  static void writeShowUnitNameFlag(bool val);

  /** \brief
   * Read the "Show LEDs with boot logo" flag in system EEPROM.
   *
   * \return `true` if the flag is set to indicate that the RGB LEDs should be
   * flashed. `false` if the flag is set to leave the LEDs off.
   *
   * \details
   * The "Show LEDs with boot logo" flag is used to determine whether the
   * RGB LEDs should be flashed in sequence while the boot logo is being
   * displayed. This function returns the value of this flag.
   *
   * \see writeShowBootLogoLEDsFlag()
   */
  static bool readShowBootLogoLEDsFlag();

  /** \brief
   * Write the "Show LEDs with boot logo" flag in system EEPROM.
   *
   * \param val If `true` the flag is set to indicate that the RGB LEDs should
   * be flashed. If `false` the flag is set to leave the LEDs off.
   *
   * \details
   * The "Show LEDs with boot logo" flag is used to determine whether the
   * RGB LEDs should be flashed in sequence while the boot logo is being
   * displayed. This function allows the flag to be saved with the desired
   * value.
   *
   * \see readShowBootLogoLEDsFlag()
   */
  static void writeShowBootLogoLEDsFlag(bool val);

  /** \brief
   * A counter which is incremented once per frame.
   *
   * \details
   * This counter is incremented once per frame when using the `nextFrame()`
   * function. It will wrap to zero when it reaches its maximum value.
   *
   * It could be used to have an event occur for a given number of frames, or
   * a given number of frames later, in a way that wouldn't be quantized the
   * way that using `everyXFrames()` might.
   *
   * example:
   * \code{.cpp}
   * // move for 10 frames when right button is pressed, if not already moving
   * if (!moving) {
   *   if (arduboy.justPressed(RIGHT_BUTTON)) {
   *     endMoving = arduboy.frameCount + 10;
   *     moving = true;
   *   }
   * } else {
   *   movePlayer();
   *   if (arduboy.frameCount == endMoving) {
   *     moving = false;
   *   }
   * }
   * \endcode
   *
   * This counter could also be used to determine the number of frames that
   * have elapsed between events but the possibility of the counter wrapping
   * would have to be accounted for.
   *
   * \see nextFrame() everyXFrames()
   */
  uint16_t frameCount;

  /** \brief
   * The display buffer array in RAM.
   *
   * \details
   * The display buffer (also known as the screen buffer) contains an
   * image bitmap of the desired contents of the display, which is written
   * to the display using the `display()` function. The drawing functions of
   * this library manipulate the contents of the display buffer. A sketch can
   * also access the display buffer directly.
   *
   * \see getBuffer()
   */
   
  // For button handling
  static uint8_t currentButtonState;
  static uint8_t previousButtonState;

 protected:
  // helper function for sound enable/disable system control
  static void sysCtrlSound();

  // functions passed to bootLogoShell() to draw the logo
  static void drawLogoBitmap();


  // For frame funcions
  uint32_t eachFrameMillis=1000/30;
  uint32_t thisFrameStart=0;
  bool justRendered;
  uint32_t lastFrameDurationMs;
};


//==============================
//========== Arduboy2 ==========
//==============================

/** \brief
 * The main functions provided for writing sketches for the Arduboy,
 * _including_ text output.
 *
 * \details
 * This class is derived from Arduboy2Base. It provides text output functions
 * in addition to all the functions inherited from Arduboy2Base.
 *
 * \note
 * A friend class named _Arduboy2Ex_ is declared by this class. The intention
 * is to allow a sketch to create an _Arduboy2Ex_ class which would have access
 * to the private and protected members of the Arduboy2 class. It is hoped
 * that this may eliminate the need to create an entire local copy of the
 * library, in order to extend the functionality, in most circumstances.
 *
 * \see Arduboy2Base
 */
class Arduboy2 : public Print, public Arduboy2Base
{
 friend class Arduboy2Ex;

 public:
  Arduboy2();

  /** \class Print
   * \brief
   * The Arduino `Print` class is available for writing text to the screen
   * buffer.
   *
   * \details
   * For an `Arduboy2` class object, functions provided by the Arduino `Print`
   * class can be used to write text to the screen buffer, in the same manner
   * as the Arduino `Serial.print()`, etc., functions.
   *
   * Print will use the `write()` function to actually draw each character
   * in the screen buffer.
   *
   * See:
   * https://www.arduino.cc/en/Serial/Print
   *
   * Example:
   * \code{.cpp}
   * int value = 42;
   *
   * arduboy.println("Hello World"); // Prints "Hello World" and then moves the
   *                                 // text cursor to the start of the next line
   * arduboy.print(value);  // Prints "42"
   * arduboy.print('\n');   // Moves the text cursor to the start of the next line
   * arduboy.print(78, HEX) // Prints "4E" (78 in hexadecimal)
   * \endcode
   *
   * \see Arduboy2::write()
   */



  /** \brief
   * Write a single ASCII character at the current text cursor location.
   *
   * \param c The ASCII value of the character to be written.
   *
   * \return The number of characters written (will always be 1).
   *
   * \details
   * This is the Arduboy implemetation of the Arduino virtual `write()`
   * function. The single ASCII character specified is written to the
   * the screen buffer at the current text cursor. The text cursor is then
   * moved to the next character position in the screen buffer. This new cursor
   * position will depend on the current text size and possibly the current
   * wrap mode.
   *
   * Two special characters are handled:
   *
   * - The newline character `\n`. This will move the text cursor to the start
   *   of the next line based on the current text size.
   * - The carriage return character `\r`. This character will be ignored.
   *
   * \note
   * This function is rather low level and, although it's available as a public
   * function, it wouldn't normally be used. In most cases the Arduino Print
   * class should be used for writing text.
   *
   * \see Print setTextSize() setTextWrap()
   */
  virtual size_t write(uint8_t);

  /** \brief
   * Draw a single ASCII character at the specified location in the screen
   * buffer.
   *
   * \param x The X coordinate, in pixels, for where to draw the character.
   * \param y The Y coordinate, in pixels, for where to draw the character.
   * \param c The ASCII value of the character to be drawn.
   * \param color the forground color of the character.
   * \param bg the background color of the character.
   * \param size The size of the character to draw.
   *
   * \details
   * The specified ASCII character is drawn starting at the provided
   * coordinate. The point specified by the X and Y coordinates will be the
   * top left corner of the character.
   *
   * \note
   * This is a low level function used by the `write()` function to draw a
   * character. Although it's available as a public function, it wouldn't
   * normally be used. In most cases the Arduino Print class should be used for
   * writing text.
   *
   * \see Print write() setTextColor() setTextBackground() setTextSize()
   */
  void drawChar(int16_t x, int16_t y, unsigned char c, uint8_t color, uint8_t bg, uint8_t size);

  /** \brief
   * Set the location of the text cursor.
   *
   * \param x The X coordinate, in pixels, for the new location of the text cursor.
   * \param y The Y coordinate, in pixels, for the new location of the text cursor.
   *
   * \details
   * The location of the text cursor is set the the specified coordinates.
   * The coordinates are in pixels. Since the coordinates can specify any pixel
   * location, the text does not have to be placed on specific rows.
   * As with all drawing functions, location 0, 0 is the top left corner of
   * the display. The cursor location will be the top left corner of the next
   * character written.
   *
   * \see getCursorX() getCursorY()
   */
  void setCursor(int16_t x, int16_t y);
  void setCursorX(int16_t x);  
  void setCursorY(int16_t y);


  /** \brief
   * Get the X coordinate of the current text cursor position.
   *
   * \return The X coordinate of the current text cursor position.
   *
   * \details
   * The X coordinate returned is a pixel location with 0 indicating the
   * leftmost column.
   *
   * \see getCursorY() setCursor()
   */
  int16_t getCursorX();

  /** \brief
   * Get the Y coordinate of the current text cursor position.
   *
   * \return The Y coordinate of the current text cursor position.
   *
   * \details
   * The Y coordinate returned is a pixel location with 0 indicating the
   * topmost row.
   *
   * \see getCursorX() setCursor()
   */
  int16_t getCursorY();

  /** \brief
   * Set the text foreground color.
   *
   * \param color The color to be used for following text.
   *
   * \see setTextBackground() getTextColor()
   */
  void setTextColor(uint8_t color);

  /** \brief
   * Get the currently set text foreground color.
   *
   * \return The color that will be used to display any following text.
   *
   * \see setTextColor()
   */
  uint8_t getTextColor();

  /** \brief
   * Set the text background color.
   *
   * \param bg The background color to be used for following text.
   *
   * \see setTextColor() getTextBackground()
   */
  void setTextBackground(uint8_t bg);

  /** \brief
   * Get the currently set text background color.
   *
   * \return The background color that will be used to display any following text.
   *
   * \see setTextBackground()
   */
  uint8_t getTextBackground();

  /** \brief
   * Set the text character size.
   *
   * \param s The text size multiplier. Must be 1 or higher.
   *
   * \details
   * Setting a text size of 1 will result in standard size characters which
   * occupy 6x8 pixels (the result of 5x7 characters with spacing on the
   * right and bottom edges).
   *
   * The value specified is a multiplier. A value of 2 will double the
   * size so they will occupy 12x16 pixels. A value of 3 will result in
   * 18x24, etc.
   *
   * \see getTextSize()
   */
  void setTextSize(uint8_t s);

  /** \brief
   * Get the currently set text size.
   *
   * \return The size that will be used for any following text.
   *
   * \see setTextSize()
   */
  uint8_t getTextSize();

  /** \brief
   * Set or disable text wrap mode.
   *
   * \param w `true` enables text wrap mode. `false` disables it.
   *
   * \details
   * Text wrap mode is enabled by specifying `true`. In wrap mode, the text
   * cursor will be moved to the start of the next line (based on the current
   * text size) if the following character wouldn't fit entirely at the end of
   * the current line.

   * If wrap mode is disabled, characters will continue to be written to the
   * same line. A character at the right edge of the screen may only be
   * partially displayed and additional characters will be off screen.
   *
   * \see getTextWrap()
   */
  void setTextWrap(bool w);

  /** \brief
   * Get the currently set text wrap mode.
   *
   * \return `true` if text wrapping is on, `false` if wrapping is off.
   *
   * \see setTextWrap()
   */
  bool getTextWrap();

  /** \brief
   * Set or disable text raw mode, allowing special characters to be displayed.
   *
   * \param raw `true` enables text raw mode. `false` disables it.
   *
   * \details
   * In text _raw_ mode, character values that would normally be treated
   * specially will instead be displayed. The special characters are:
   *
   * - ASCII newline/line feed (`\n`, 0x0A, inverse white circle).
   * - ASCII carriage return (`\r`, 0x0D, musical eighth note).
   *
   * All other characters can be displayed regardless of whether raw mode
   * is enabled or not.
   *
   * \see getTextRawMode() Print
   */
  void setTextRawMode(bool raw);

  /** \brief
   * Get the current state of text raw mode.
   *
   * \return `true` if text raw mode is enabled, `false` if disabled.
   *
   * \see setTextRawMode()
   */
  bool getTextRawMode();


  /** \brief
   * Clear the display buffer and set the text cursor to location 0, 0
   */
  void clear();

 /** \brief
   * Get the width, in pixels, of a character in the library's font.
   *
   * \param textSize The text size the character would be drawn at
   * (optional; defaults to 1).
   *
   * \return The width, in pixels, that a character will occupy, not including
   * inter-character spacing.
   *
   * \details
   * Returns the width, in pixels, occupied by a character in the font used by
   * the library for text functions. The result will be based on the provided
   * text size, or size 1 if not included. Since the font is monospaced, all
   * characters will occupy the same width for a given text size.
   *
   * The width does not include the spacing added after each character by the
   * library text functions. The `getCharacterSpacing()` function can be used
   * to obtain the character spacing value.
   *
   * \see getCharacterHeight() getCharacterSpacing()
   * getTextSize() setTextSize() font5x7
   */
  uint8_t getCharacterWidth(uint8_t textSize = 1)
  {
    return characterWidth * textSize;
  }

  /** \brief
   * Get the number of pixels added after each character to provide spacing.
   *
   * \param textSize The text size the character would be drawn at
   * (optional; defaults to 1).
   *
   * \return The number of pixels of space added after each character.
   *
   * \details
   * Returns the number of pixels added to the right of each character,
   * to provide spacing, when drawn by the library text functions.
   * The result will be based on the provided text size, or size 1 if not
   * included.
   *
   * \see getCharacterWidth() getLineSpacing()
   * getTextSize() setTextSize() font5x7
   */
  uint8_t getCharacterSpacing(uint8_t textSize = 1)
  {
    return characterSpacing * textSize;
  }

  /** \brief
   * Get the height, in pixels, of a character in the library's font.
   *
   * \param textSize The text size the character would be drawn at
   * (optional; defaults to 1).
   *
   * \return The height, in pixels, that a character will occupy.
   *
   * \details
   * Returns the height, in pixels, that a character will occupy when drawn
   * using the library text functions. The result will be based on the
   * provided text size, or size 1 if not included.
   *
   * \see getCharacterWidth() getLineSpacing()
   * getTextSize() setTextSize() font5x7
   */
  uint8_t getCharacterHeight(uint8_t textSize = 1)
  {
    return characterHeight * textSize;
  }

  /** \brief
   * Get the number of pixels added below each character to provide
   * line spacing.
   *
   * \param textSize The text size the character would be drawn at
   * (optional; defaults to 1).
   *
   * \return The number of pixels of space added below each character.
   *
   * \details
   * Returns the number of pixels added below each character, to provide
   * spacing for wrapped lines, when drawn by the library text functions.
   * The result will be based on the provided text size, or size 1 if
   * not included.
   *
   * \note
   * For this library, the value returned will be 0 because no spacing is added
   * between lines. This function is included so that it can be used to write
   * code that would be easily portable for use with a suite of equivalent
   * functions that rendered text with added line spacing.
   *
   * \see getCharacterHeight() getCharacterSpacing()
   * getTextSize() setTextSize() font5x7
   */
  uint8_t getLineSpacing(uint8_t textSize = 1)
  {
    return lineSpacing * textSize;
  }

  /** \brief
   * The font used for text functions.
   *
   * \details
   * This is a 5 pixel by 7 pixel font. Each character is actually coded as
   * 8 pixels high to allow a 1 pixel descender below the baseline.
   * Many symbols also use the 8th pixel. The library functions add a 1 pixel
   * space after each character to separate them, so characters written at
   * size 1 will occupy a 6 x 8 pixel area when drawn.
   *
   * The character set represented is code page 437, also known as OEM 437,
   * OEM-US, PC-8 or DOS Latin US. This is an 8 bit set which includes all
   * printable ASCII characters plus many accented characters, symbols and
   * line drawing characters.
   *
   * The data for this font is from file `glcdfont.c` in the
   * [Adafruit GFX graphics library](https://github.com/adafruit/Adafruit-GFX-Library).
   *
   * \note
   * \parblock
   * With the library's text functions, the line drawing characters in the font
   * won't touch on the left and right sides, as originally intended, because
   * of the extra blank pixel added to the right of each character.
   * \endparblock
   *
   * \note
   * \parblock
   * The library's text functions, except `drawChar()`, handle two character
   * values specially:
   *
   * - ASCII newline/line feed (`\n`, 0x0A, inverse white circle).
   *   This will move the text cursor position to the start of the next line,
   *   based on the current text size.
   * - ASCII carriage return (`\r`, 0x0D, musical eighth note).
   *   This character will be ignored.
   *
   * To override the special handling of the above values, to allow the
   * characters they represent to be printed, text _raw_ mode can be selected
   * using the `setTextRawMode()` function.
   * \endparblock
   *
   * \see Print write() drawChar() setTextRawMode()
   * getCharacterWidth() getCharacterHeight()
   * getCharacterSpacing() getLineSpacing() readUnitName() writeUnitName()
   */
  uint8_t * font5x7 = (uint8_t *)&font[0];


 protected:
  int16_t cursor_x;
  int16_t cursor_y;
  uint8_t textColor;
  uint8_t textBackground;
  uint8_t textSize;
  bool textWrap;
  bool textRaw;

  // Width and height of a font5x7 character
  // (not including inter-character spacing)
  uint8_t characterWidth = 5;
  uint8_t characterHeight = 8;
  // Width of inter-character spacing
  uint8_t characterSpacing = 1;
  // Height of inter-line spacing
  uint8_t lineSpacing = 0;
  // Character sizes including spacing
  uint8_t fullCharacterWidth = characterWidth + characterSpacing;
  uint8_t fullCharacterHeight = characterHeight + lineSpacing;
  
  
};

#endif

