#include <FastLED.h>
#include <Ewma.h>
#define LED_PIN     2
#define NUM_LEDS    24
#define BRIGHTNESS 128
CRGB leds[NUM_LEDS];
#define POT_BRIGHT_PIN  6
#define POT_COLOUR_PIN 5

int oldInputBright;
int oldInputColour;

Ewma adcFilter1(0.1);  //Bigger is less smoothing!!!!
Ewma adcFilter2(0.1);  //Bigger is worse !!


int led = 6; // the pin the LED is connected to
int led2 = 7;
int led3 = 9;
int led4 = 10;
int led5 = 11;
int led6 = 12;







void setup() {


  Serial.begin(9600);  //we dont need this in the final thing
  oldInputBright = map(analogRead(POT_BRIGHT_PIN), 0, 1023, 0, 128);//
  int oldInputColour = map(analogRead(POT_COLOUR_PIN), 0, 1023, 0, 256);




  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);


  //for (int i = 0 ; i < 25 ; i++) {        //THIS IS NOT NEEDED AFTER COLOUR POT IS ADDED!!!!!
  //  leds[i] = CRGB(255, 0, 255);

  //  FastLED.show();
 // }

  pinMode(led, OUTPUT); // Declare the LED as an output
  pinMode(led2, OUTPUT); //
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode(led6, OUTPUT);


}


void loop() {



  
  int inputBrightness = map(analogRead(POT_BRIGHT_PIN), 0, 1023, 0, 129);
  int smoothedInputBrightness = adcFilter1.filter(inputBrightness);


  if (smoothedInputBrightness != oldInputBright) {
    changeBrightness(smoothedInputBrightness);
    Serial.println(smoothedInputBrightness);
    oldInputBright = smoothedInputBrightness;
  }



  int inputColour = map(analogRead(POT_COLOUR_PIN), 0, 1023, 0, 128);
  int smoothedInputColour = adcFilter2.filter(inputColour);


  if (smoothedInputColour == 127) {
    changeBrightness(0);
    thunder();
    Serial.println("THUNDER");
   
  }


  else if (smoothedInputColour != oldInputColour) {
    changeBrightness(smoothedInputBrightness);

    changeColour(smoothedInputColour*2);
    oldInputColour = smoothedInputColour;
  }


  delay(20);
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


void thunder() {
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

    }

    delay(random(20, 100));
    digitalWrite(led2, LOW);
    digitalWrite(led, LOW); // Turn the LED on
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
    digitalWrite(led5, LOW);
    digitalWrite(led6, LOW);
    delay(random(50, 500));
  }


}
