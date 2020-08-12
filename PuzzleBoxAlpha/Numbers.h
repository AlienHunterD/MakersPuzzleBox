// Contains all of the Character shapes for led display to be used with DisplayChar function


// Reference: {[0 1 2 3 4 5 6 7 8 9 10 11] [0 1 2 3 4 5 6]}
//              Ring leds ^^^              Jewl leds^^^ 

bool* NUMS[10];

bool ZERO[] {true, true, false, false, false, true, true, true, false, false, false, true, false, false, true, true, false, true, true};
bool ONE[] {true, false, false, false, false, true, true, true, false, false, false, true, true, true, false, false, true, false, false};
bool TWO[] {true, true, false, false, false, true, true, true, false, false, false, true, true, false, true, false, false, true, false};
bool THREE[] {true, true, false, false, false, true, true, true, false, false, false, true, true, false, true, true, false, false, false};
bool FOUR[] {false, true, false, false, false, true, false, false, false, false, false, true, true, false, true, true, false, false, true};
bool FIVE[] {true, true, false, false, false, true, true, true, false, false, false, true, true, false, false, true, false, false, true};
bool SIX[] {true, true, false, false, false, true, true, true, false, false, false, true, true, false, false, true, false, true, true};
bool SEVEN[] {true, true, false, false, false, true, false, false, false, false, false, true, false, false, true, true, false, false, false};
bool EIGHT[] {true, true, false, false, false, true, true, true, false, false, false, true, true, false, true, true, false, true, true};
bool NINE[] {true, true, false, false, false, true, false, false, false, false, false, true, true, false, true, true, false, false, true};

bool I[] {true, true, false, false, false, true, true, true, false, false, false, true, true, true, false, false, true, false, false};
bool E[] {true, true, false, false, false, true, true, true, false, false, false, true, true, false, false, false, false, true, true};
