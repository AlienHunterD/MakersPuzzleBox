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
#include "Numbers.h";

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
enum HintType {Solid, FlashOrder, DisplayNumber};
enum ControlsType {Simple, Random, SimpleAdjacent, RandomAdjacent};
enum GoalType {SolidG, RandomG, Bianary};

// Setting up the library for the different neopixel arrays
Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ring(RINGPIXELS, RING_PIN, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel jewl(JEWLPIXELS, JEWL_PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels
int puzzleLevel = 0; // Start at level '0'
int BianaryGoalNum = 0; // Used for bianary puzzles
int lightArray[]={0,1,2,3,4,5,6,7}; // Array that holds the index number of the lights
bool goal[] = {true, true, true, true, true, true, true, true}; // Initialize goal value for the light puzzle
int bias[] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW}; // Include Bias in the light state
bool current[] = {false, false, false, false, false, false, false, false}; // Initialize starting light state

PuzzleBoxState currentState = Waiting; // Start in the waiting state prior to play.

// Initialize type variables
HintType currentHint = Solid;
ControlsType currentControls = Simple;
GoalType currentGoal = SolidG;

void SeedRNG()
{
  // Read noise from the analog channels to seed the random number generator
  int r = 0;
  for(int i=A0; i<=A3; i++)
    r += analogRead(i);
  randomSeed(r);
}


void SetControls()
{
  //NEEDS SOME WORK
  SetAllPixels(jewl, JEWLPIXELS, 150, 0, 0); // Turn jewl red
  jewl.show();
  
  switch (currentControls)
  { 
    case Simple:
      for (int i = 0; i < NUMPIXELS; i++)
        lightArray[i] = i;

      // First level "shuffle" animation
      for (int i = 0; i < NUMPIXELS * 2; i++){
        pixels.setBrightness(i * 7);
        ring.setBrightness(i * 7);
        SetAllPixels(ring, RINGPIXELS, 0, 0, 150);
        SetAllPixels(pixels, NUMPIXELS, 0, 0, 150);
        pixels.show();
        ring.show();
        delay(60);
      }
      break;

    case Random:
      // Shuffle the contents of the light array to confuse the mapping of the switches to lights
      for(int repeat=0; repeat < RINGPIXELS; repeat++)  
      { 
        ring.setPixelColor(repeat, ring.Color(0, 0, 150)); // Make loading ring
        ring.show();
    
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
  }
  SetAllPixels(jewl, JEWLPIXELS, 0, 0, 150); // Turn jewl blue
  jewl.show();
}


void ReadSwitches()
{
  // Check starting state of switches to set bias so all switches begin in state "false"
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
  
  SeedRNG(); // Randomize the RNG so we get actual random numbers

  // INITIALIZE NeoPixel strip objects (REQUIRED)
  pixels.begin(); 
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


void SetAllPixels(Adafruit_NeoPixel &strip, int numPix, int r, int g, int b)
{
  // Function to easily set all pixels in a strip to one color
  for(int led = 0; led < numPix; led++)
    {
      strip.setPixelColor(led, strip.Color(r, g, b));
    }  
  return;
}


void DisplayNum(bool num[])
{
  for (int i = 0; i < RINGPIXELS; i++)
  {
    if (num[i])
      ring.setPixelColor(i, ring.Color(0, 150, 0));
    else
      ring.setPixelColor(i, ring.Color(10, 0, 0));  
  }
  for (int i = 0; i < JEWLPIXELS; i++)
  {
    if (num[i + 12])
      jewl.setPixelColor(i, jewl.Color(0, 150, 0));
    else
      jewl.setPixelColor(i, jewl.Color(10, 0, 0));  
  }
  ring.show();
  jewl.show();
}


void Reset()
{ 
  // Turn off all leds and perform "boot up" light display to test neopixels
  puzzleLevel = 0; // Initialize puzzleLevel to first level (0)
  ChangeState(Waiting); // Move to the waiting state
  for(int i = 0; i < 3; i++) // Blink three times
  {
    // Set all pixel colors to 'off'
    pixels.clear(); 
    ring.clear();
    jewl.clear();
    pixels.show();
    ring.show();
    jewl.show();

    // Just blink yellow for now
    SetAllPixels(pixels, NUMPIXELS, 100, 100, 0);
    SetAllPixels(ring, RINGPIXELS, 100, 100, 0);
    SetAllPixels(jewl, JEWLPIXELS, 100, 100, 0);
    pixels.show();
    ring.show();
    jewl.show();
    
    delay(250);
    
    pixels.clear();
    ring.clear();
    jewl.clear();
    pixels.show();
    ring.show();
    jewl.show();
    
    delay(250);
  }
}


void loop() 
{
  // Main game state loop
  switch(currentState)
  {
    case Waiting:
      // State between games
      UpdateWaiting();
      break;

    case Starting:
      ChangeState(Playing);
      SetupLevel();
      break;

    case Playing:
      UpdatePlay();
      break;

    case Won:
      UpdateWon();
      break; 
  }
}


void ChangeState(PuzzleBoxState newState)
{
  // Handle all the transitions needed to change to a new state
  currentState = newState;
  switch(newState)
  {
    case Playing:
      // Handle the transition into the playing state
      ReadSwitches();
      break;

    case Waiting:
      // Get ready to wait some
      break;
  }
}


void UpdateWaiting()
{
  // Clear all pixels
  pixels.clear(); 
  ring.clear();  
  jewl.clear();
  ring.show(); 
  pixels.show();  
  jewl.show();
  
  ring.setBrightness(75); // Turn down the brightness
  jewl.setBrightness(75); 
  
  int pin = 0;
  int red = 0, green = 0, blue = 0; 
  int pled;
  int jled;
  for(int rled = 0; rled < RINGPIXELS; rled++)
  { 
    // Flash random colors on all pixels while waiting
    // Since the light arrays are of differing lenghts, use remainder of index to avoid using three 'for' loops
    pled = rled % NUMPIXELS; 
    jled = rled % JEWLPIXELS;

    // Pick random colors for current pixel
    red = random(0,150);
    green = random(0,150);
    blue = random(0,150);

    // Set all of the pixel colors
    pixels.setPixelColor(pled, pixels.Color(red, green, blue));
    ring.setPixelColor(rled, pixels.Color(red, green, blue));
    jewl.setPixelColor(jled, pixels.Color(red, green, blue));
  }
  
  ring.show(); 
  pixels.show();  
  jewl.show();

  // Check if button is pressed
  if(digitalRead(BUTTON_PIN) == LOW)
  {
    // If pressed, start the game
    ChangeState(Starting);
    return;
  }
  // Time between random pixel color changes 
  delay(100);
}


void SetGoal()
{

  switch (currentGoal)
  {
    case SolidG:
      for (int i = 0; i < NUMPIXELS; i++)
        goal[i] = true;
      break;
      
    case RandomG:
      // Randomize the goal array
      int value;
      for (int i = 0; i < NUMPIXELS; i++)
      {
          value = (random(100) > 50);
          goal[i] = value;
      }
      break;

    case Bianary:
      // Generate a random number between 1 and 128
      //NEEDS TESTING
      BianaryGoalNum = random(1, 128);
      int temp = BianaryGoalNum;
      for (int i = 8; i > 0; i--)
      { 
        goal[i] = (temp % 2);  
        temp = temp / 2;
      }
  }
  return;
}


void SetupLevel()
{
  // Set up the puzzle depending on the current puzzleLevel
  switch (puzzleLevel)
  {
    case 0:
    currentHint = Solid;
    currentControls = Simple;
    currentGoal = SolidG;
    break;

    case 1:
    currentHint = FlashOrder;
    currentControls = Simple;
    currentGoal = RandomG;
    break;

    case 2:
    currentHint = FlashOrder;
    currentControls = Random;
    currentGoal = RandomG;
    break;

    case 3:
    currentHint = FlashOrder;
    currentControls = SimpleAdjacent;
    currentGoal = RandomG;
    break;

    case 4:
    currentHint = FlashOrder;
    currentControls = RandomAdjacent;
    currentGoal = RandomG;
    break;

    case 5:
    currentHint = DisplayNumber;
    currentControls = Simple;
    currentGoal = Bianary;
    break;

    case 6:
    currentHint = DisplayNumber;
    currentControls = Random;
    currentGoal = Bianary;
    break;

    case 7:
    currentHint = DisplayNumber;
    currentControls = SimpleAdjacent;
    currentGoal = Bianary;
    break;

    case 8:
    currentHint = DisplayNumber;
    currentControls = RandomAdjacent;
    currentGoal = Bianary;
    break;
  }

  SetControls();
  SetGoal();
}


void DisplayHint()
{
  switch (currentHint)
  {
    case Solid:
      SetAllPixels(jewl, JEWLPIXELS, 0, 150, 0);
      SetAllPixels(ring, RINGPIXELS, 0, 150, 0);
      ring.show();
      jewl.show();
      delay(750);
      
      break;

    case FlashOrder:
      for (int i=0; i < NUMPIXELS; i++)
      {
        if (goal[i])
        {
          SetAllPixels(jewl, JEWLPIXELS, 0, 150, 0);
          SetAllPixels(ring, RINGPIXELS, 0, 150, 0);
        }
        else
        {
          SetAllPixels(jewl, JEWLPIXELS, 150, 0, 0);
          SetAllPixels(ring, RINGPIXELS, 150, 0, 0);
        }
        jewl.show();
        ring.show();
        UpdatePixelState(); // So that there is not a long time where the state of the pixels does not match the state of the switches
        delay(400);
        jewl.clear();
        ring.clear();
        jewl.show();
        ring.show();
        UpdatePixelState();
        delay(250);
      }
      break;
      
    case DisplayNumber:
      // Need to develop number displays first.
      break;
  }

  delay(250);
  SetAllPixels(jewl, JEWLPIXELS, 0, 0, 150);
  SetAllPixels(ring, RINGPIXELS, 0, 0, 150);
  jewl.show();
  ring.show();
  return;
}


void UpdatePixelState()
{
  // Update pixels based on current switch state
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
  return;
}


void UpdatePlay()
{ 
  // While playing, change lights who's corresponding switches have been switched and check for win
  UpdatePixelState();

  if(ReachedTheGoal())
  {
    ChangeState(Won);
    puzzleLevel++;
  }

  if(digitalRead(BUTTON_PIN) == LOW)
  {
    DisplayHint();
  }
  delay(100);
}


void SetLightValue(int light, bool value)
{
  // Used to change lights from their "true" and "false" states.
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
  // Check if current state of lights match the goal state
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
  // Flash purple until button is pressed
  if(digitalRead(BUTTON_PIN) == LOW)
  {
    ChangeState(Starting);
    return;
  }
  pixels.clear();
  pixels.show();
  delay(250);

  if(digitalRead(BUTTON_PIN) == LOW) // Check button is pressed twice so that there is not a half a second where you can't press the button
  {
    ChangeState(Starting);
    return;
  }
  
  SetAllPixels(pixels, NUMPIXELS, 150, 0, 150);
  pixels.show();
  delay(250);
}
