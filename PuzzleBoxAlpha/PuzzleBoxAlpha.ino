/********************************************************************************
 * IEEE UH-Makers Puzzle Box Alpha Mk 1 v 0.1
 * Alex Bradshaw ???
 * Dan Biediger debiediger@uh.edu
 * Spring 2020
 * 
 * This is the code for the simple prototype puzzle box using switches,
 * lights, and a button. It contains the code to run a set of simple games
 * using the lights and binary number conversions as well as a "mastermind"
 * syle puzzle.
 ********************************************************************************/

#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define LED_PIN 1 
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 8 // To represent one word length
// Where does the ribbon connected to the switches start?
#define SWITCH_PIN_START 2

// Setting up the library with 8 pixels in a strip
Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels

int lightArray[]={0,1,2,3,4,5,6,7}; // Array that holds the index number of the lights
bool goal[] = {true, false, false, true, true, false, false, true}; // Goal value for the light puzzle
bool current[] = {false, false, false, false, false, false, false, false}; // Current light state

// Read noise from the analog channels to seed the random number generator
void SeedRNG()
{
  int r = 0;
  for( int i=A0; i<=A3; i++)
    r += analogRead( i);
  randomSeed(r);
}

void setup() 
{
  // Randomize the RNG so we get actual random numbers
  SeedRNG();

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)

  for(int pin = SWITCH_PIN_START; pin < SWITCH_PIN_START+NUMPIXELS; pin++)
  {
    pinMode(pin, INPUT_PULLUP); // All pins in pull up mode
  }

  // Shuffle the contents of the light array to confuse the mapping of the switches to lights
  for(int repeat=0; repeat<6; repeat++)  
  {
    for (int i=0; i < NUMPIXELS; i++) 
    {
      pixels.clear();
      int n = random(0, NUMPIXELS);  // Integer from 0 to the number of pixels-1
      int temp = lightArray[n];
      lightArray[n] =  lightArray[i];
      lightArray[i] = temp;

      // Show the lights that have been swapped momentarily in blue
      pixels.setPixelColor(lightArray[i], pixels.Color(0, 0, 150)); 
      pixels.setPixelColor(lightArray[n], pixels.Color(0, 0, 150)); 
      pixels.show();
      delay(50);
    }
  }
}

void loop() 
{
  pixels.clear(); // Set all pixel colors to 'off'
  int pin = 0;
  for(int led = 0; led < NUMPIXELS; led++)
  {
    pin = led + SWITCH_PIN_START; 
    if (digitalRead(pin) == LOW) 
    {
      SetLightValue(lightArray[led], true);  
    }
    else
    {
      SetLightValue(lightArray[led], false);
    }
  }
  
  pixels.show();   // Send the updated pixel colors to the hardware.

  ScoreThePuzzle();
  
  delay(100);
}

void SetLightValue(int light, bool value)
{
  if(value)
  {
    pixels.setPixelColor(light, pixels.Color(0, 150, 0));
    current[light] = true;
  }
  else
  {
    pixels.setPixelColor(light, pixels.Color(150, 0, 0));
    current[light] = false;
  }
}

void ScoreThePuzzle()
{
  for(int i=0; i < NUMPIXELS; i++)
  {
    if(current[i] != goal[i])
    {
      return;
    }
  }
  while(true)
  {
    pixels.clear();
    delay(500);
    for(int i = 0; i<NUMPIXELS; i++)
    {
      pixels.setPixelColor(i, pixels.Color(150, 0, 150));
    }
   
    pixels.show();
    delay(500);
  }
}
