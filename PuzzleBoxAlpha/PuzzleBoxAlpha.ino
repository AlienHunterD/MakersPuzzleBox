/********************************************************************************
 * IEEE UH-Makers Puzzle Box Alpha Mk 1 v 0.2
 * Alex Bradshaw azbradshaw@uh.edu
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
#define LED_PIN 11
#define RING_PIN 12
#define JEWL_PIN 13
#define BUTTON_PIN 10
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 8 // To represent one word length
#define RINGPIXELS 12
#define JEWLPIXELS 7
// Where does the ribbon connected to the switches start?
#define SWITCH_PIN_START 2

enum PuzzleBoxState {Starting, Playing, Won, Lost, Waiting};

// Setting up the library with 8 pixels in a strip
Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ring(RINGPIXELS, RING_PIN, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel jewl(JEWLPIXELS, JEWL_PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels
int puzzleLevel = 0; 
int lightArray[]={0,1,2,3,4,5,6,7}; // Array that holds the index number of the lights
bool goal[] = {true, false, false, true, true, false, false, true}; // Goal value for the light puzzle
int bias[] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW}; // Include Bias in the light state
bool current[] = {false, false, false, false, false, false, false, false}; // Current light state

PuzzleBoxState currentState = Waiting; // Start in the waiting state prior to play.

// Read noise from the analog channels to seed the random number generator
void SeedRNG()
{
  int r = 0;
  for( int i=A0; i<=A3; i++)
    r += analogRead( i);
  randomSeed(r);
}

void Shuffle()
{
  // Shuffle the contents of the light array to confuse the mapping of the switches to lights
  ring.clear();
  
  for(int repeat=0; repeat < RINGPIXELS; repeat++)  
  { 
    ring.setPixelColor(repeat, ring.Color(0, 0, 150)); // Make loading ring
    ring.show();
    
    for (int led = 0; led < JEWLPIXELS; led++)
    {
      jewl.setPixelColor(led, jewl.Color(150, 0, 0)); // Turn jewl red
    } 
    jewl.show();
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
      delay(15);
    }
  }
  for (int led = 0; led < JEWLPIXELS; led++)
    {
      jewl.setPixelColor(led, jewl.Color(0, 0, 150)); // Turn jewl blue
    } 
  jewl.show();
}
void ReadSwitches()
{
  for (int i = 0; i < NUMPIXELS; i++)
  {
    if (digitalRead(i + SWITCH_PIN_START) == HIGH)
    {
        bias[i] = LOW;
    }
    else
      bias[i] = HIGH;  
  }
}

void setup() 
{
  // Randomize the RNG so we get actual random numbers
  SeedRNG();

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  ring.begin();
  jewl.begin();
  
  // Set the intial pin modes
  for(int pin = SWITCH_PIN_START; pin < SWITCH_PIN_START+NUMPIXELS; pin++)
  {
    pinMode(pin, INPUT_PULLUP); // All pins in pull up mode
  }

   pinMode(BUTTON_PIN, INPUT_PULLUP);

  Reset();
}

void Reset()
{
  puzzleLevel = 0;
  ChangeState(Waiting); // Move to the waiting state
  for(int i = 0; i < 3; i++) // Blink three times
  {
    pixels.clear(); // Set all pixel colors to 'off'
    ring.clear();
    jewl.clear();
    pixels.show();
    ring.show();
    jewl.show();
    
    for(int led = 0; led < NUMPIXELS; led++)
    {
      pixels.setPixelColor(led, pixels.Color(100, 100, 0));
    }
    pixels.show();
    delay(250);
    pixels.clear();
    pixels.show();
    delay(250);
  }
  
}

void loop() 
{
  switch(currentState)
  {
    case Waiting:
      UpdateWaiting();
      break;

    case Starting:
      ChangeState(Playing);
      break;

    case Playing:
      UpdatePlay();
      break;

    case Won:
      UpdateWon();
      break; 
  }
}

// Handle all the transitions needed to change to a new state
void ChangeState(PuzzleBoxState newState)
{
  currentState = newState;
  switch(newState)
  {
    case Playing:
      Shuffle();
      ReadSwitches();
      // Handle the transition into the playing state
      break;

    case Waiting:
      // Get ready to wait some
      break;
  }
  
}

void UpdateWaiting()
{
  pixels.clear(); // Set all pixel colors to 'off'
  ring.clear();  // Set all pixels in the ring to 'off'

  ring.setBrightness(75); // Turn down the brightness
  jewl.setBrightness(75); 
  int pin = 0;
  int red = 0, green = 0, blue = 0; 
  int pled;
  int jled;
  for(int rled = 0; rled < RINGPIXELS; rled++)
  { 
    pled = rled % NUMPIXELS;
    jled = rled % JEWLPIXELS;
    
    red = random(0,150);
    green = random(0,150);
    blue = random(0,150);
    
    pixels.setPixelColor(pled, pixels.Color(red, green, blue));
    ring.setPixelColor(rled, pixels.Color(red, green, blue));
    jewl.setPixelColor(jled, pixels.Color(red, green, blue));
  }
  
  ring.show(); // Update the ring of pixels;
  pixels.show();   // Send the updated pixel colors to the hardware.
  jewl.show();
  
  if(digitalRead(BUTTON_PIN) == LOW)
  {
    ChangeState(Starting);
    return;
  }
  
  delay(100);
}

void UpdatePlay()
{
  pixels.clear(); // Set all pixel colors to 'off'
  int pin = 0;
  for(int toggle = 0; toggle < NUMPIXELS; toggle++)
  {
    pin = toggle + SWITCH_PIN_START; 
    if (digitalRead(pin) == bias[toggle]) 
    {
      SetLightValue(lightArray[toggle], true);  
    }
    else
    {
      SetLightValue(lightArray[toggle], false);
    }
  }
  
  pixels.show();   // Send the updated pixel colors to the hardware.

  if(ReachedTheGoal())
  {
    ChangeState(Won);
  }
  
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

bool ReachedTheGoal()
{
  for(int i=0; i < NUMPIXELS; i++)
  {
    if(current[i] != goal[i])
    {
      return false;
    }
  }
  return true;
}

void UpdateWon()
{
  if(digitalRead(BUTTON_PIN) == LOW)
  {
    ChangeState(Starting);
    return;
  }
  pixels.clear();
  pixels.show();
  delay(250);
  
  for(int i = 0; i<NUMPIXELS; i++)
  {
    pixels.setPixelColor(i, pixels.Color(150, 0, 150));
  }
  pixels.show();
  delay(250);
}
