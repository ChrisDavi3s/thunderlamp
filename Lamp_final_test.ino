#include <FastLED.h>
#include <Ewma.h>
#define LED_PIN     2
#define POT_LEFT  6
#define POT_RIGHT 5
#define NUM_LEDS      24
#define LED_TYPE   WS2811
#define COLOR_ORDER   GRB
#define DATA_PIN        2
//#define CLK_PIN       4
#define VOLTS          12
#define MAX_MA       4000
CRGBArray<NUM_LEDS> leds;
#define TWINKLE_SPEED 3
#define TWINKLE_DENSITY 7
#define SECONDS_PER_PALETTE  1    //not needed 
CRGB gBackgroundColor = CRGB::Black;
#define AUTO_SELECT_BACKGROUND_COLOR 0
#define COOL_LIKE_INCANDESCENT 0
int counter;
CRGBPalette16 gCurrentPalette;
CRGBPalette16 gTargetPalette;


//CRGB leds[NUM_LEDS];
int led = 2; // the pin the LED is connected to
int led2 = 7;
int led3 = 6;
int led4 = 3;
int led5 = 4;
int led6 = 8;
int led7 = 9;

Ewma adcFilter1(0.2);  //Bigger is less smoothing!!!!
Ewma adcFilter2(0.05);

void setup() {
  Serial.begin(9600);
  //chooseNextColorPalette(gTargetPalette,1);


  FastLED.setMaxPowerInVoltsAndMilliamps( VOLTS, MAX_MA);
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  // FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(255);
  pinMode(led, OUTPUT); // Declare the LED as an output
  pinMode(led2, OUTPUT); //
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode(led6, OUTPUT);
  pinMode(led7, OUTPUT);
}

void changeColour(int c ) {
  FastLED.setBrightness(255);
  FastLED.clear();
  CHSV colour = CHSV(c * 2, 255, 255);
  fill_solid(leds, NUM_LEDS, colour);
  FastLED.show();
}

void allWhite(int b) {
  FastLED.clear();
  CHSV colour = CHSV(255, 0, b * 2);
  fill_solid(leds, NUM_LEDS, colour);
  FastLED.show();
}

void lightning() {
  FastLED.clear();
  FastLED.show();

  for (int r = 0; r < random(2, 10); r++) {
    //iterate through every LED
    for (int i = 0; i < 2; i++) {
      if (random(0, 100) > 90) {
        digitalWrite(led, HIGH); // Turn the LED on

      }
      else if (random(0, 100) > 90) {
        digitalWrite(led2, HIGH);
      }
      else if (random(0, 100) > 90) {
        digitalWrite(led3, HIGH);
      }
      else if (random(0, 100) > 90) {
        digitalWrite(led4, HIGH);
      }
      else if (random(0, 100) > 90) {
        digitalWrite(led5, HIGH);
      }
      else if (random(0, 100) > 90) {
        digitalWrite(led6, HIGH);
      }
      else if (random(0, 100) > 90) {
        digitalWrite(led7, HIGH);
      }
    }

    delay(random(20, 100));
    digitalWrite(led2, LOW);
    digitalWrite(led, LOW); // Turn the LED on
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
    digitalWrite(led5, LOW);
    digitalWrite(led6, LOW);
    digitalWrite(led7, LOW);
    delay(random(50, 500));
  }
}




void drawTwinkles( CRGBSet& L)
{
  // "PRNG16" is the pseudorandom number generator
  // It MUST be reset to the same starting value each time
  // this function is called, so that the sequence of 'random'
  // numbers that it generates is (paradoxically) stable.
  uint16_t PRNG16 = 11337;
  
  uint32_t clock32 = millis();

  // Set up the background color, "bg".
  // if AUTO_SELECT_BACKGROUND_COLOR == 1, and the first two colors of
  // the current palette are identical, then a deeply faded version of
  // that color is used for the background color
  CRGB bg;
  if( (AUTO_SELECT_BACKGROUND_COLOR == 1) &&
      (gCurrentPalette[0] == gCurrentPalette[1] )) {
    bg = gCurrentPalette[0];
    uint8_t bglight = bg.getAverageLight();
    if( bglight > 64) {
      bg.nscale8_video( 16); // very bright, so scale to 1/16th
    } else if( bglight > 16) {
      bg.nscale8_video( 64); // not that bright, so scale to 1/4th
    } else {
      bg.nscale8_video( 86); // dim, scale to 1/3rd.
    }
  } else {
    bg = gBackgroundColor; // just use the explicitly defined background color
  }

  uint8_t backgroundBrightness = bg.getAverageLight();
  
  for( CRGB& pixel: L) {
    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
    uint16_t myclockoffset16= PRNG16; // use that number as clock offset
    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
    // use that number as clock speed adjustment factor (in 8ths, from 8/8ths to 23/8ths)
    uint8_t myspeedmultiplierQ5_3 =  ((((PRNG16 & 0xFF)>>4) + (PRNG16 & 0x0F)) & 0x0F) + 0x08;
    uint32_t myclock30 = (uint32_t)((clock32 * myspeedmultiplierQ5_3) >> 3) + myclockoffset16;
    uint8_t  myunique8 = PRNG16 >> 8; // get 'salt' value for this pixel

    // We now have the adjusted 'clock' for this pixel, now we call
    // the function that computes what color the pixel should be based
    // on the "brightness = f( time )" idea.
    CRGB c = computeOneTwinkle( myclock30, myunique8);

    uint8_t cbright = c.getAverageLight();
    int16_t deltabright = cbright - backgroundBrightness;
    if( deltabright >= 32 || (!bg)) {
      // If the new pixel is significantly brighter than the background color, 
      // use the new color.
      pixel = c;
    } else if( deltabright > 0 ) {
      // If the new pixel is just slightly brighter than the background color,
      // mix a blend of the new color and the background color
      pixel = blend( bg, c, deltabright * 8);
    } else { 
      // if the new pixel is not at all brighter than the background color,
      // just use the background color.
      pixel = bg;
    }
  }
}


//  This function takes a time in pseudo-milliseconds,
//  figures out brightness = f( time ), and also hue = f( time )
//  The 'low digits' of the millisecond time are used as 
//  input to the brightness wave function.  
//  The 'high digits' are used to select a color, so that the color
//  does not change over the course of the fade-in, fade-out
//  of one cycle of the brightness wave function.
//  The 'high digits' are also used to determine whether this pixel
//  should light at all during this cycle, based on the TWINKLE_DENSITY.
CRGB computeOneTwinkle( uint32_t ms, uint8_t salt)
{
  uint16_t ticks = ms >> (8-TWINKLE_SPEED);
  uint8_t fastcycle8 = ticks;
  uint16_t slowcycle16 = (ticks >> 8) + salt;
  slowcycle16 += sin8( slowcycle16);
  slowcycle16 =  (slowcycle16 * 2053) + 1384;
  uint8_t slowcycle8 = (slowcycle16 & 0xFF) + (slowcycle16 >> 8);
  
  uint8_t bright = 0;
  if( ((slowcycle8 & 0x0E)/2) < TWINKLE_DENSITY) {
    bright = attackDecayWave8( fastcycle8);
  }

  uint8_t hue = slowcycle8 - salt;
  CRGB c;
  if( bright > 0) {
    c = ColorFromPalette( gCurrentPalette, hue, bright, NOBLEND);
    if( COOL_LIKE_INCANDESCENT == 1 ) {
      coolLikeIncandescent( c, fastcycle8);
    }
  } else {
    c = CRGB::Black;
  }
  return c;
}


// This function is like 'triwave8', which produces a 
// symmetrical up-and-down triangle sawtooth waveform, except that this
// function produces a triangle wave with a faster attack and a slower decay:
//
//     / \ 
//    /     \ 
//   /         \ 
//  /             \ 
//

uint8_t attackDecayWave8( uint8_t i)
{
  if( i < 86) {
    return i * 3;
  } else {
    i -= 86;
    return 255 - (i + (i/2));
  }
}

// This function takes a pixel, and if its in the 'fading down'
// part of the cycle, it adjusts the color a little bit like the 
// way that incandescent bulbs fade toward 'red' as they dim.
void coolLikeIncandescent( CRGB& c, uint8_t phase)
{
  if( phase < 128) return;

  uint8_t cooling = (phase - 128) >> 4;
  c.g = qsub8( c.g, cooling);
  c.b = qsub8( c.b, cooling * 2);
}

// A mostly red palette with green accents and white trim.
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 RedGreenWhite_p FL_PROGMEM =
{  CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red, 
   CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red, 
   CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray, 
   CRGB::Green, CRGB::Green, CRGB::Green, CRGB::Green };

// A mostly (dark) green palette with red berries.
#define Holly_Green 0x00580c
#define Holly_Red   0xB00402
const TProgmemRGBPalette16 Holly_p FL_PROGMEM =
{  Holly_Green, Holly_Green, Holly_Green, Holly_Green, 
   Holly_Green, Holly_Green, Holly_Green, Holly_Green, 
   Holly_Green, Holly_Green, Holly_Green, Holly_Green, 
   Holly_Green, Holly_Green, Holly_Green, Holly_Red 
};

// A red and white striped palette
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 RedWhite_p FL_PROGMEM =
{  CRGB::Red,  CRGB::Red,  CRGB::Red,  CRGB::Red, 
   CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray,
   CRGB::Red,  CRGB::Red,  CRGB::Red,  CRGB::Red, 
   CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray };

// A mostly blue palette with white accents.
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 BlueWhite_p FL_PROGMEM =
{  CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, 
   CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, 
   CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, 
   CRGB::Blue, CRGB::Gray, CRGB::Gray, CRGB::Gray };

// A pure "fairy light" palette with some brightness variations
#define HALFFAIRY ((CRGB::FairyLight & 0xFEFEFE) / 2)
#define QUARTERFAIRY ((CRGB::FairyLight & 0xFCFCFC) / 4)
const TProgmemRGBPalette16 FairyLight_p FL_PROGMEM =
{  CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, 
   HALFFAIRY,        HALFFAIRY,        CRGB::FairyLight, CRGB::FairyLight, 
   QUARTERFAIRY,     QUARTERFAIRY,     CRGB::FairyLight, CRGB::FairyLight, 
   CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight };

// A palette of soft snowflakes with the occasional bright one
const TProgmemRGBPalette16 Snow_p FL_PROGMEM =
{  0x304048, 0x304048, 0x304048, 0x304048,
   0x304048, 0x304048, 0x304048, 0x304048,
   0x304048, 0x304048, 0x304048, 0x304048,
   0x304048, 0x304048, 0x304048, 0xE0F0FF };

// A palette reminiscent of large 'old-school' C9-size tree lights
// in the five classic colors: red, orange, green, blue, and white.
#define C9_Red    0xB80400
#define C9_Orange 0x902C02
#define C9_Green  0x046002
#define C9_Blue   0x070758
#define C9_White  0x606820
const TProgmemRGBPalette16 RetroC9_p FL_PROGMEM =
{  C9_Red,    C9_Orange, C9_Red,    C9_Orange,
   C9_Orange, C9_Red,    C9_Orange, C9_Red,
   C9_Green,  C9_Green,  C9_Green,  C9_Green,
   C9_Blue,   C9_Blue,   C9_Blue,
   C9_White
};

// A cold, icy pale blue palette
#define Ice_Blue1 0x0C1040
#define Ice_Blue2 0x182080
#define Ice_Blue3 0x5080C0
const TProgmemRGBPalette16 Ice_p FL_PROGMEM =
{
  Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
  Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
  Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
  Ice_Blue2, Ice_Blue2, Ice_Blue2, Ice_Blue3
};


// Add or remove palette names from this list to control which color
// palettes are used, and in what order.
const TProgmemRGBPalette16* ActivePaletteList[] = {
  &RetroC9_p,
  &BlueWhite_p,
  &RainbowColors_p,
  &FairyLight_p,
  &RedGreenWhite_p,
  &PartyColors_p,
  &RedWhite_p,
  &Snow_p,
  &Holly_p,
  &Ice_p  
};


// Advance to the next color palette in the list (above).
void chooseNextColorPalette( CRGBPalette16& pal,int c)
{
  pal = *(ActivePaletteList[c]);

  
  FastLED.clear();
  for (uint8_t i=0; i<=NUM_LEDS; i++) {
    uint8_t paletteIndex = map(i,0,NUM_LEDS-1,0,240);  //index is now based on pixel number
    leds[i] = ColorFromPalette( gTargetPalette, paletteIndex, 255, LINEARBLEND);
  }
  FastLED.show();
  delay(1000);

  
}







int multiMap(int val, int* _in, int* _out, uint8_t size)
{
  // take care the value is within range
  // val = constrain(val, _in[0], _in[size-1]);
  if (val <= _in[0]) return _out[0];
  if (val >= _in[size-1]) return _out[size-1];

  // search right interval
  uint8_t pos = 1;  // _in[0] allready tested
  while(val > _in[pos]) pos++;

  // this will handle all exact "points" in the _in array
  if (val == _in[pos]) return _out[pos];

  // interpolate in the right segment for the rest
  return (val - _in[pos-1]) * (_out[pos] - _out[pos-1]) / (_in[pos] - _in[pos-1]) + _out[pos-1];
}
















void loop() {
  //int inputLeft = map(analogRead(POT_LEFT), 0, 1023, 0, 4);
  //int smoothedLeft = adcFilter1.filter(inputLeft);

  int inputRight = map(analogRead(POT_RIGHT), 0, 1023, 0, 128);
  int smoothedRight = adcFilter2.filter(inputRight);
  int smoothedRightFade = map(smoothedRight,0,128,0,10);

  int out[] = {0,1,2,3};
  int in[]  = { 0, 15,100,256};


  int inputLeft = adcFilter1.filter(analogRead(POT_LEFT));
  int smoothedLeft = multiMap( inputLeft, in, out, 4);

  
  
  
 // int inputLeft = multiMap( analogRead(POT_LEFT), in, out, 4);
  //int smoothedLeft = adcFilter1.filter(inputLeft);
  Serial.println(inputLeft);
  
  
  
  
  
  switch (smoothedLeft) {
    case 0 :
    changeColour(smoothedRight);
      break;
    case 1 : allWhite(smoothedRight);
      break;
    case 2 : lightning();
      break;
    case 3:
  
   
      if (counter!=smoothedRightFade)
      {

        counter=smoothedRightFade;

        //gTargetPalette = *ActivePaletteList[smoothedRightFade];
        
        chooseNextColorPalette( gTargetPalette,smoothedRightFade );
        
      }
    
      
  

     EVERY_N_MILLISECONDS( 10 ) {
        nblendPaletteTowardPalette( gCurrentPalette, gTargetPalette, 12);
      }

      drawTwinkles(leds);
      

      FastLED.show();
      break;
  }

//  delay(10);

}
