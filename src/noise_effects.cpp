#include <Arduino.h>
#include "functions.h"

#if (USE_NOISE_EFFECTS == 1)

// The 16 bit version of our coordinates
static uint16_t x;
static uint16_t y;
static uint16_t z;

uint16_t speed = 20; // speed is set dynamically once we've started up
uint16_t scale = 30; // scale is set dynamically once we've started up

// This is the array that we keep our computed noise values in
#define MAX_DIMENSION (max(WIDTH, HEIGHT))
#if (WIDTH > HEIGHT)
uint8_t noise[WIDTH][WIDTH];
#else
uint8_t noise[HEIGHT][HEIGHT];
#endif

CRGBPalette16 currentPalette( PartyColors_p );
uint8_t colorLoop = 1;
uint8_t ihue = 0;

void lavaNoise(the_lamp_state_t *lamp_state)
{
  if (lamp_state->loadingFlag)
  {
    lamp_state->loadingFlag = false;

    currentPalette = LavaColors_p;
    speed = LAVA_SPEED; 
    scale = lamp_state->scale /*LAVA_SCALE*/; 
    colorLoop = 0;
  }
  fillNoiseLED(lamp_state->leds);
}

void cloudNoise(the_lamp_state_t *lamp_state) 
{
  if (lamp_state->loadingFlag)
  {
    lamp_state->loadingFlag = false;

    currentPalette = CloudColors_p;
    speed = CLOUD_SPEED; 
    scale = lamp_state->scale /*CLOUD_SCALE*/;
    colorLoop = 0;
  }
  fillNoiseLED(lamp_state->leds);
}

void zebraNoise(the_lamp_state_t *lamp_state)
{
  if (lamp_state->loadingFlag)
  {
    lamp_state->loadingFlag = false;

    SetupBlackAndWhiteStripedPalette();       
    speed = ZEBRA_SPEED; 
    scale = lamp_state->scale /*ZEBRA_SCALE*/; 
    colorLoop = 1;
  }
  fillNoiseLED(lamp_state->leds);
}

void forestNoise(the_lamp_state_t *lamp_state)
{
  if (lamp_state->loadingFlag)
  {
    lamp_state->loadingFlag = false;

    currentPalette = ForestColors_p;
    speed = FOREST_SPEED; 
    scale = lamp_state->scale /*FOREST_SCALE*/; 
    colorLoop = 0;
  }
  fillNoiseLED(lamp_state->leds);
}

void oceanNoise(the_lamp_state_t *lamp_state)
{
  if (lamp_state->loadingFlag)
  {
    lamp_state->loadingFlag = false;

    currentPalette = OceanColors_p;
    speed = OCEAN_SPEED; 
    scale = lamp_state->scale /*OCEAN_SCALE*/; 
    colorLoop = 0;
  }

  fillNoiseLED(lamp_state->leds);
}

void plasmaNoise(the_lamp_state_t *lamp_state)
{
  if (lamp_state->loadingFlag)
  {
    lamp_state->loadingFlag = false;

    currentPalette = PartyColors_p;
    speed = PLASMA_SPEED; 
    scale = lamp_state->scale /*PLASMA_SCALE*/; 
    colorLoop = 1;
  }
  fillNoiseLED(lamp_state->leds);
}

void plumNoise(the_lamp_state_t *lamp_state)
{
  if (lamp_state->loadingFlag)
  {
    lamp_state->loadingFlag = false;

    SetupPurpleAndGreenPalette();       
    speed = PLUM_SPEED; 
    scale = lamp_state->scale /*PLUM_SCALE*/; 
    colorLoop = 1;
  }
  fillNoiseLED(lamp_state->leds);
}

void randomNoise(the_lamp_state_t *lamp_state)
{
  if (lamp_state->loadingFlag)
  {
    lamp_state->loadingFlag = false;

    SetupRandomPalette();       
    speed = RANDOM_SPEED; 
    scale = lamp_state->scale /*RANDOM_SCALE*/; 
    colorLoop = 1;
  }
  fillNoiseLED(lamp_state->leds);
}

void rainbowNoise(the_lamp_state_t *lamp_state)
{
  if (lamp_state->loadingFlag)
  {
    lamp_state->loadingFlag = false;

    currentPalette = RainbowColors_p;
    speed = RANDOM_SPEED; 
    scale = lamp_state->scale /*RANDOM_SCALE*/; 
    colorLoop = 1;
  }
  fillNoiseLED(lamp_state->leds);
}

void rainbowStripeNoise(the_lamp_state_t *lamp_state)
{
  if (lamp_state->loadingFlag)
  {
    lamp_state->loadingFlag = false;

    currentPalette = RainbowStripeColors_p;
    speed = RAINBOW_STR_SPEED; 
    scale = lamp_state->scale /*RAINBOW_STR_SCALE*/; 
    colorLoop = 1;
  }
  fillNoiseLED(lamp_state->leds);
}

///////////////////////////////////////////////////////////////////////////////////
// Auxiliary functions from NoisePlusPalette.ino (from fastLED library examples) //
///////////////////////////////////////////////////////////////////////////////////

// This function generates a random palette that's a gradient
// between four different colors.  The first is a dim hue, the second is 
// a bright hue, the third is a bright pastel, and the last is 
// another bright hue.  This gives some visual bright/dark variation
// which is more interesting than just a gradient of different hues.
void SetupRandomPalette()
{
  currentPalette = CRGBPalette16( 
                      CHSV( random8(), 255, 32), 
                      CHSV( random8(), 255, 255), 
                      CHSV( random8(), 128, 255), 
                      CHSV( random8(), 255, 255)); 
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
  // 'black out' all 16 palette entries...
  fill_solid( currentPalette, 16, CRGB::Black);
  // and set every fourth one to white.
  currentPalette[0] = CRGB::White;
  currentPalette[4] = CRGB::White;
  currentPalette[8] = CRGB::White;
  currentPalette[12] = CRGB::White;
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
  CRGB purple = CHSV( HUE_PURPLE, 255, 255);
  CRGB green  = CHSV( HUE_GREEN, 255, 255);
  CRGB black  = CRGB::Black;
  
  currentPalette = CRGBPalette16( 
    green,  green,  black,  black,
    purple, purple, black,  black,
    green,  green,  black,  black,
    purple, purple, black,  black );
}

void fillNoiseLED(CRGB *leds)
{
  fillnoise8();
  mapNoiseToLEDsUsingPalette(leds);
}

///////////////////////////////////////////////////////////////////////////////////
// The basic functions of rendering effects on the matrix                        //
///////////////////////////////////////////////////////////////////////////////////

// Fill the x/y array of 8-bit noise values using the inoise8 function.
void fillnoise8() {
  // If we're runing at a low "speed", some 8-bit artifacts become visible
  // from frame-to-frame.  In order to reduce this, we can do some fast data-smoothing.
  // The amount of data smoothing we're doing depends on "speed".
  uint8_t dataSmoothing = 0;
  if (speed < 50)
    dataSmoothing = 200 - (uint8_t)(speed << 2); // dataSmoothing = 200 - (speed * 4);
  
  for (int i = 0; i < MAX_DIMENSION; i++) {
    int ioffset = scale * i;
    for(int j = 0; j < MAX_DIMENSION; j++) {
      int joffset = scale * j;
      
      uint8_t data = inoise8(x + ioffset, y + joffset, z);

      // The range of the inoise8 function is roughly 16-238.
      // These two operations expand those values out to roughly 0..255
      // You can comment them out if you want the raw noise data.
      data = qsub8(data,16);
      data = qadd8(data,scale8(data,39));

      if( dataSmoothing ) {
        uint8_t olddata = noise[i][j];
        uint8_t newdata = scale8( olddata, dataSmoothing) + scale8( data, 256 - dataSmoothing);
        data = newdata;
      }
      
      noise[i][j] = data;
    }
  }
  
  z += speed;
  
  // apply slow drift to X and Y, just for visual variation.
  x += (speed >> 3); // x += speed / 8;
  y -= (speed >> 4); // y -= speed / 16;
}

void mapNoiseToLEDsUsingPalette(CRGB *leds) {
  for(int i = 0; i < WIDTH; i++) {
    for(int j = 0; j < HEIGHT; j++) {
      // We use the value at the (i,j) coordinate in the noise
      // array for our brightness, and the flipped value from (j,i)
      // for our pixel's index into the color palette.

      uint8_t index = noise[j][i];
      uint8_t bri   = noise[i][j];

      // if this palette is a 'loop', add a slowly-changing base value
      if (colorLoop)
        index += ihue;

      // brighten up, as the color palette itself often contains the 
      // light/dark dynamic range desired
      if (bri > 127)
        bri = 255;
      else
        bri = dim8_raw(bri << 1); // bri = dim8_raw( bri * 2);

      CRGB color = ColorFromPalette(currentPalette, index, bri);
      drawPixelXY(leds, i, j, color);
    }
  }
  
  ihue++;
}

#else // #if (USE_NOISE_EFFECTS == 1)

void lavaNoise()
{
  return;
}
void cloudNoise()
{
  return;
}
void zebraNoise()
{
  return;
}
void forestNoise()
{
  return;
}
void oceanNoise()
{
  return;
}
void plasmaNoise()
{
  return;
}
void plumNoise()
{
  return;
}
void randomNoise()
{
  return;
}
void rainbowNoise()
{
  return;
}
void rainbowStripeNoise()
{
  return;
}

#endif // #if (USE_NOISE_EFFECTS == 1)
