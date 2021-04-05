#include <FastLED.h>
#include <Ewma.h>
#define LED_PIN     2
#define NUM_LEDS    24
#define BRIGHTNESS 128
CRGB leds[NUM_LEDS];
#define POT_BRIGHT_PIN  5
#define POT_COLOUR_PIN 6

int oldInputBright;
int oldInputColour;

Ewma adcFilter1(0.4);  //Bigger is less smoothing!!!!
Ewma adcFilter2(0.4);







void setup() {
  pinMode(A3, INPUT);
  Serial.begin(9600);  //we dont need this in the final thing
  oldInputBright = map(analogRead(POT_BRIGHT_PIN), 0, 1023, 0, 128);//
  int oldInputColour = map(analogRead(POT_COLOUR_PIN), 0, 1023, 0, 256);




  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);


  for (int i = 0 ; i < 25 ; i++) {        //THIS IS NOT NEEDED AFTER COLOUR POT IS ADDED!!!!!
    leds[i] = CRGB(255, 0, 255);

    FastLED.show();
  }




}


void loop() {





  int inputBrightness = map(analogRead(POT_BRIGHT_PIN), 0, 1023, 0, 128);
  int smoothedInputBrightness = adcFilter1.filter(inputBrightness);
  if (smoothedInputBrightness != oldInputBright) {
    changeBrightness(smoothedInputBrightness);
    Serial.println(smoothedInputBrightness);
    oldInputBright = smoothedInputBrightness;
  }



  int inputColour = map(analogRead(POT_COLOUR_PIN), 0, 1023, 0, 256);
  int smoothedInputColour = adcFilter2.filter(inputColour);
  if (smoothedInputColour != oldInputColour) {
    changeColour(smoothedInputColour);
    oldInputColour = smoothedInputColour;
  }


  delay(30);
}



void changeColour(int c) {
  Serial.println(c);
  FastLED.clear();
  int s = 255;
  if (c == 0) {
    s = 0;
  }
  CHSV colour = CHSV(c, s, 255);
  fill_solid(leds, NUM_LEDS, colour);
  FastLED.show();

}

void changeBrightness(int brightness)
{
  Serial.println(brightness);
  FastLED.setBrightness(brightness);
  FastLED.show();
}
