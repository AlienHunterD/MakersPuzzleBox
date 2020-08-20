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
#include "Numbers.h"; // Number Shapes File

// Define Pin Locations
#define STRIP_PIN 11
#define RING_PIN 12
#define JEWL_PIN 13
#define BUTTON_PIN 10
#define SWITCH_PIN_START 2 // Beginning of Switch Pin Ribbon

// Define Length of Each Neopixel Array
#define STRIP_LENGTH 8 
#define RING_LENGTH 12
#define JEWL_LENGTH 7


// Setting up the libraries for the different neopixel arrays
Adafruit_NeoPixel strip(STRIP_LENGTH, STRIP_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ring(RING_LENGTH, RING_PIN, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel jewl(JEWL_LENGTH, JEWL_PIN, NEO_GRB + NEO_KHZ800);


// Puzzle State & Difficulty Variables
enum PuzzleBoxState {Starting, Playing, Won, Lost, Waiting}; // Main state of puzzle box
enum HintType {Solid, FlashOrder, DisplayNumber}; // What happens when the button is pressed during the game
enum ControlsType {Simple, Random, SimpleAdjacent, RandomAdjacent}; // How the switches interact with the lights
enum GoalType {SolidG, RandomG, Binary}; // How to test for the goal



// Initialization of Puzzle Variables
int puzzleLevel = 0; // Start at level '0'
int BinaryGoalNum = 0; // Used for bianary puzzles
int GoalDigits[3] = {0, 0, 0};
int lightArray[]={0,1,2,3,4,5,6,7}; // Array that holds the index number of the lights
bool goal[] = {true, true, true, true, true, true, true, true}; // Initialize goal value for the light puzzle
int bias[] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW}; // Include Bias in the light state
bool current[] = {false, false, false, false, false, false, false, false}; // Initialize starting light state
bool switchStates[] = {false, false, false, false, false, false, false, false}; // Initialize array for keeping track of state of switches
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


// ANIMATIONS & DISPLAY----------------------------------------------------------

// Default Display Colors
void DisplayChar(bool c[], int rf = 0, int gf = 25, int bf = 0, int rb = 10, int gb = 0, int bb = 0);

void FadeIn()
{
  // All pixels fade in from dark
  for (int i = 0; i < STRIP_LENGTH * 2; i++)
  {
        strip.setBrightness(i * 7);
        ring.setBrightness(i * 7);
        jewl.setBrightness(i*7);
        SetAllPixels(ring, RING_LENGTH, 0, 0, 100);
        SetAllPixels(strip, STRIP_LENGTH, 0, 0, 100);
        SetAllPixels(jewl, JEWL_LENGTH, 0, 0, 100);
        strip.show();
        ring.show();
        jewl.show();
        delay(60);
  }
}


void TestDisplay()
{
  jewl.clear();
  jewl.show();

  float colorValue = 0;
  while (colorValue < 10)
  {
     SetAllPixels(ring, RING_LENGTH, colorValue, 0, 0);
     SetAllPixels(jewl, JEWL_LENGTH, colorValue, 0, 0);
     ring.show();
     jewl.show();
     colorValue += 0.01;
  }
  
  for(int i = 0; i < STRIP_LENGTH/2; i++)
  {
    strip.setPixelColor(i, strip.Color(0, 0, 50));
    strip.setPixelColor(STRIP_LENGTH - 1 - i, strip.Color(0, 0, 50));
    strip.show();
    if (i == 0)
      DisplayChar(I);
    else 
      DisplayChar(E);
    
    delay(500);
    SetAllPixels(ring, RING_LENGTH, 10, 0, 0);
    SetAllPixels(jewl, JEWL_LENGTH, 10, 0, 0);
    ring.show();
    jewl.show();
    delay(250);
  }
  
}

void WaitingAnimation()
{
// Clear all pixels
  strip.clear(); 
  ring.clear();  
  jewl.clear();
  ring.show(); 
  strip.show();  
  jewl.show();

  // Turn down the brightness
  ring.setBrightness(75); 
  jewl.setBrightness(75); 
  strip.setBrightness(75);
  
  int pin = 0;
  int red = 0, green = 0, blue = 0; 
  int pled;
  int jled;
  for(int rled = 0; rled < RING_LENGTH; rled++)
  { 
    // Flash random colors on all pixels while waiting
    // Since the light arrays are of differing lenghts, use remainder of index to avoid using three 'for' loops
    pled = rled % STRIP_LENGTH; 
    jled = rled % JEWL_LENGTH;

    // Pick random colors for current pixel
    red = random(0,75);
    green = random(0,75 - red);
    blue = random(0,75 - red - green);

    // Set all of the pixel colors
    strip.setPixelColor(pled, strip.Color(red, green, blue));
    ring.setPixelColor(rled, strip.Color(red, green, blue));
    jewl.setPixelColor(jled, strip.Color(red, green, blue));
  }
  
  ring.show(); 
  strip.show();  
  jewl.show();  
  
}

void SetAllPixels(Adafruit_NeoPixel &strip, int numPix, float r, float g, float b)
{
  // Function to easily set all pixels in a strip to one color
  for(int led = 0; led < numPix; led++)
    {
      strip.setPixelColor(led, strip.Color(r, g, b));
    }  
  return;
}


void DisplayChar(bool c[], int rf, int gf, int bf, int rb, int gb, int bb)
{
  // Uses number variables in "Numbers.h" file to display number on circular display
  
  // Set ring lights
  for (int i = 0; i < RING_LENGTH; i++)
  {
    if (c[i])
      ring.setPixelColor(i, ring.Color(rf, gf, bf));
    else
      ring.setPixelColor(i, ring.Color(rb, gb, bb));  
  }
  // Set jewl lights
  for (int i = 0; i < JEWL_LENGTH; i++)
  {
    if (c[i + 12])
      jewl.setPixelColor(i, jewl.Color(rf, gf, bf));
    else
      jewl.setPixelColor(i, jewl.Color(rb, gb, bb));  
  }
  
  ring.show();
  jewl.show();
}


// GAME-----------------------------------------------------------------------

void setup() 
{

  Serial.begin(9600);

  // INITIALIZE NeoPixel strip objects (REQUIRED)
  strip.begin(); 
  ring.begin();
  jewl.begin();

  SeedRNG(); // Randomize the RNG so we get actual random numbers
  
  // Define NUMS
  NUMS[0] = ZERO;
  NUMS[1] = ONE;
  NUMS[2] = TWO;
  NUMS[3] = THREE;
  NUMS[4] = FOUR;
  NUMS[5] = FIVE;
  NUMS[6] = SIX;
  NUMS[7] = SEVEN;
  NUMS[8] = EIGHT;
  NUMS[9] = NINE;
  
  // Set the intial pin modes
  for(int pin = SWITCH_PIN_START; pin < SWITCH_PIN_START+STRIP_LENGTH; pin++)
  {
    pinMode(pin, INPUT_PULLUP); // All pins in pull up mode
  }
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Clear Pixels
  jewl.clear();
  ring.clear();
  strip.clear();
  jewl.show();
  strip.show();
  ring.show();
  
  // Wait for initial button press (show pre-boot animation)
  float colorValue1 = 60;
  float colorValue2 = 30;
  float colorValue3 = 0;
  float c1 = 0;
  float c2 = 0;
  float c3 = 0;
  bool up1 = true;
  bool up2 = true;
  bool up3 = true;
  while (digitalRead(BUTTON_PIN) == HIGH)
  {
    if (colorValue1 >= 100)
      up1 = false;
    else if (colorValue1 <= -30)
      up1 = true;
    if (up1)
      colorValue1 += 0.15;
    else
      colorValue1 -= 0.15;
      
    if (colorValue2 >= 100)
      up2 = false;
    else if (colorValue2 <= -30)
      up2 = true;
    if (up2)
      colorValue2 += 0.15;
    else
      colorValue2 -= 0.15;
      
    if (colorValue3 >= 100)
      up3 = false;
    else if (colorValue3 <= -30)
      up3 = true;
    if (up3)
      colorValue3 += 0.15;
    else
      colorValue3 -= 0.15;
    c1 = colorValue1;
    c2 = colorValue2;
    c3 = colorValue3;
    
    if (colorValue3 < 0)
       c3 = 0;
    if (colorValue2 < 0)
      c2 = 0;
    if (colorValue1 < 0)
      c1 = 0;
    SetAllPixels(ring, RING_LENGTH, 0, c3, 0);
    SetAllPixels(jewl, JEWL_LENGTH, 0, c2, 0);
    jewl.setPixelColor(0, jewl.Color(0, c1, 0));
    ring.show();
    jewl.show();  
  }
  
  Reset(); //Begin Boot Up
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


void Reset()
{ 
  // Turn off all leds and perform "boot up" light display to test neopixels
  puzzleLevel = 0; // Initialize puzzleLevel to first level (0)
  ChangeState(Waiting); // Move to the waiting state
  TestDisplay();
}


void SetupLevel()
{
  // Set up the puzzle depending on the current puzzleLevel
  // Set the difficulty of each level here
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
    currentHint = DisplayNumber;
    currentControls = Simple;
    currentGoal = Binary;
    break;
    
    case 4:
    currentHint = DisplayNumber;
    currentControls = Random;
    currentGoal = Binary;
    break;

    case 5:
    currentHint = FlashOrder;
    currentControls = SimpleAdjacent;
    currentGoal = RandomG;
    break;

    case 6:
    currentHint = FlashOrder;
    currentControls = RandomAdjacent;
    currentGoal = RandomG;
    break;

    case 7:
    currentHint = DisplayNumber;
    currentControls = SimpleAdjacent;
    currentGoal = Binary;
    break;

    case 8:
    currentHint = DisplayNumber;
    currentControls = RandomAdjacent;
    currentGoal = Binary;
    break;
  }

  SetControls();
  SetGoal();
}


void SetControls()
{
  // Based on currentControls, set the lightArray so that the indexes correspond to the desired switches
  
  SetAllPixels(jewl, JEWL_LENGTH, 100, 0, 0); // Turn jewl red
  jewl.show();
  
  switch (currentControls)
  { 
    case Simple:
    case SimpleAdjacent:
    // One-for-One Correspondance
      for (int i = 0; i < STRIP_LENGTH; i++)
        lightArray[i] = i;
        
      FadeIn(); // Animation
      break;

    case Random:
    case RandomAdjacent:
      // Shuffle the contents of the light array to confuse the mapping of the switches to lights
      FadeIn();
      SetAllPixels(ring, RING_LENGTH, 100, 0, 0);
      for(int repeat=0; repeat < RING_LENGTH; repeat++)  
      { 
        ring.setPixelColor(repeat, ring.Color(0, 0, 100)); // Make loading ring
        ring.show();
    
        for (int i=0; i < STRIP_LENGTH; i++) 
        {
          strip.clear();
          int n = random(0, STRIP_LENGTH);  // Integer from 0 to the number of pixels-1
          int temp = lightArray[n];
          lightArray[n] =  lightArray[i];
          lightArray[i] = temp;
    
          // Show the lights that have been swapped momentarily in blue
          strip.setPixelColor(lightArray[i], strip.Color(0, 0, 100)); 
          strip.setPixelColor(lightArray[n], strip.Color(0, 0, 100)); 
          strip.show();
          delay(15);
        }
      }
      break;
  }
  
  SetAllPixels(jewl, JEWL_LENGTH, 0, 0, 100); // Turn jewl blue
  jewl.show();
}


void SetGoal()
{
  // Using the currentGoal variable, set the goal state of the lights
  switch (currentGoal)
  {
    case SolidG:
      // All on
      for (int i = 0; i < STRIP_LENGTH; i++)
        goal[i] = true;
      break;

    case RandomG:
      // Randomize the goal array
      int value;
      for (int i = 0; i < STRIP_LENGTH; i++)
      {
          value = (random(100) > 50);
          goal[i] = value;
      }
      break;

    case Binary:
      // Generate a random number between 1 and 128
      BinaryGoalNum = random(1, 255);
      int temp = BinaryGoalNum;
      for (int i = 0; i < 3; i++)
      {
        GoalDigits[i] = temp % 10;
        temp = temp / 10;
      }
      temp = BinaryGoalNum;
      for (int i = 7; i >= 0; i--)
      { 
        goal[i] = (temp % 2);  
        temp = temp / 2;
      }
  }
  return;
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


void ReadSwitches()
{
  // Check starting state of switches to set bias so all switches begin in "false" state
  for (int i = 0; i < STRIP_LENGTH; i++)
  {
    if (digitalRead(i + SWITCH_PIN_START) == HIGH)
    {
        bias[i] = LOW;
    }
    else
      bias[i] = HIGH;  
  }
}


void UpdateWaiting()
{
  WaitingAnimation();

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


void DisplayHint()
{
  // When button is pressed, display hint on circular display depending on currentHint variable
  switch (currentHint)
  {
    case Solid:
      // Just turn display green
      SetAllPixels(jewl, JEWL_LENGTH, 0, 100, 0);
      SetAllPixels(ring, RING_LENGTH, 0, 100, 0);
      ring.show();
      jewl.show();
      delay(750);
      break;

    case FlashOrder:
      // Flash the goal on the circular display
      for (int i=0; i < STRIP_LENGTH; i++)
      {
        if (goal[i])
        {
          SetAllPixels(jewl, JEWL_LENGTH, 0, 100, 0);
          SetAllPixels(ring, RING_LENGTH, 0, 100, 0);
        }
        else
        {
          SetAllPixels(jewl, JEWL_LENGTH, 100, 0, 0);
          SetAllPixels(ring, RING_LENGTH, 100, 0, 0);
        }
        jewl.show();
        ring.show();
        UpdatePixelState(); // So that there is not a long time where the state of the pixels does not match the state of the switches
        delay(400);
        jewl.clear();
        ring.clear();
        jewl.show();
        ring.show();
        UpdatePixelState(); //^^
        delay(250);
      }
      break;
      
    case DisplayNumber:
      for (int i = 2; i >= 0; i--)
      {
        if ((i == 2 && GoalDigits[i] == 0) || (i == 1 && GoalDigits[i] == 0 &&  GoalDigits[2] == 0))
        {
          continue; 
        }
        DisplayChar(NUMS[GoalDigits[i]]);
        UpdatePixelState();
        delay(400);
        jewl.clear();
        ring.clear();
        jewl.show();
        ring.show();
        UpdatePixelState();
        delay(250);
      }
      break;
  }

  delay(250);
  // Set display back to blue
  SetAllPixels(jewl, JEWL_LENGTH, 0, 0, 100);
  SetAllPixels(ring, RING_LENGTH, 0, 0, 100);
  jewl.show();
  ring.show();
  return;
}


void UpdatePixelState()
{
  // Update pixels based on current switch state and control type
  strip.clear(); // Set all pixel colors to 'off'
  int pin = 0;
  // Read Switches
  for(int toggle = 0; toggle < STRIP_LENGTH; toggle++)
  {
    pin = toggle + SWITCH_PIN_START; 
    if (digitalRead(pin) == bias[toggle]) 
    {
      switchStates[toggle] = true;
    }
    else
    {
      switchStates[toggle] = false;
    }
    SetLightValue(lightArray[toggle], switchStates[toggle]); // Update Light Array
  }

  // If in adjacent control mode
  if ((currentControls == SimpleAdjacent || currentControls == RandomAdjacent))
  {
    for(int toggle = 0; toggle < STRIP_LENGTH; toggle++)
    {
      // If either previous or next switch is on
      if (switchStates[toggle + 1] ^ switchStates[toggle - 1])
      {
        SetLightValue(lightArray[toggle], !switchStates[toggle]); // Toggle light  
      }
    }
    // End Cases
    if (switchStates[0] ^ switchStates[6])
    {
      SetLightValue(lightArray[7], !switchStates[7]);
    }
    else
    {
      SetLightValue(lightArray[7], switchStates[7]);
    }
    if (switchStates[1] ^ switchStates[7])
    {
      SetLightValue(lightArray[0], !switchStates[0]);
    }
    else
    {
      SetLightValue(lightArray[0], switchStates[0]);
    }
  }

  
  strip.show();   // Send the updated pixel colors to the hardware.  
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

  // If the button is pressed during play, display hint
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
    strip.setPixelColor(light, strip.Color(0, 100, 0));
    current[light] = true;
  }
  else
  {
    strip.setPixelColor(light, strip.Color(100, 0, 0));
    current[light] = false;
  }
}


bool ReachedTheGoal()
{
  // Check if current state of lights match the goal state
  for(int i=0; i < STRIP_LENGTH; i++)
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
  strip.clear();
  strip.show();
  delay(250);

  if(digitalRead(BUTTON_PIN) == LOW) // Check button is pressed twice so that there is not a half a second where you can't press the button
  {
    ChangeState(Starting);
    return;
  }
  
  SetAllPixels(strip, STRIP_LENGTH, 75, 0, 100);
  strip.show();
  delay(250);
}
