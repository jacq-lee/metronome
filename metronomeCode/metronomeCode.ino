// METRONOME TIMING VARIABLES
int bpm;

// LED AND SPEAKER PINS
// const int metronomeLed = ;
// const int metronomeSpeaker = ;
const int measureLed = 9;
const int increaseLed = 5;
const int decreaseLed = 11;

// LED STATES
int metronomeLedState = LOW;
int measureLedState = LOW;
int increaseLedState = LOW;
int decreaseLedState = LOW;

// BUTTON PINS
const int measureButton = 7;
// const int noteButton = ;
const int increaseButton = 10;
const int decreaseButton = 12;

// BUTTON STATES IN VOID LOOP
bool buttonState;
bool lastButtonState;
bool measureButtonState;
bool lastMeasureButtonState;
bool increaseButtonState;
bool lastIncreaseButtonState;
bool decreaseButtonState;
bool lastDecreaseButtonState;

// INTERRUPT VARIABLES
const int anyButton = 2;
volatile byte measureState;
volatile byte increaseState;
volatile byte decreaseState;

// DEBOUNCE VARIABLES
  // Button state variables used in debounce 
bool newDebounceState;
bool lastDebounceState;
bool currentDebounceState;
  // Timing variables used in debounce
unsigned long lastButtonChangeTime = 0;
unsigned long debounceDuration = 50;

// FUNCTIONS:
// void loop
  // light up the led in accordance to the time signature
// read a button (debounce)
// set the measure and note length
// increase the bpm
// decrease the bpm

// interrupt
  // after the measure button? or increase/decrease buttons have been pushed, set the new measure time?


void setup() {
  // put your setup code here, to run once:
  // BUTTON PINS
  pinMode(anyButton, INPUT);
  pinMode(measureButton, INPUT);
  // pinMode(noteButton, INPUT);
  pinMode(increaseButton, INPUT);
  pinMode(decreaseButton, INPUT);

  // LED AND SPEAKER PINS
  // pinMode(metronomeLed, OUTPUT);
  // pinMode(metronomeSpeaker, OUTPUT);
  pinMode(measureLed, OUTPUT);
  pinMode(increaseLed, OUTPUT);
  pinMode(decreaseLed, OUTPUT);

  // INTERRUPT FUNCTION CALL
  attachInterrupt(digitalPinToInterrupt(anyButton), buttonPress, RISING);
}


void loop() {
  // put your main code here, to run repeatedly:
  measureButtonState = debounce( digitalRead(measureButton) );
    
  if(measureButtonState == HIGH && lastMeasureButtonState == LOW) {
    measureLedState = !measureLedState;
    digitalWrite(measureLed, measureLedState);
  }
  lastMeasureButtonState = measureButtonState;
}


int increaseBPM( int bpm )
{
  
  while( debounce( digitalRead(measureButton))== LOW ){
    digitalWrite( increaseLed, HIGH );
    bpm += 5;
    delay(500); // use timer here
    // call function that increases the bpm
  }
  digitalWrite(increaseLed, LOW) ;
  return bpm;
}

// same as IncreaseBPM but bpm is decreasing instead
int decreaseBPM(int bpm)
{
  while(debounce(measureButton)== LOW){
    digitalWrite(decreaseLed, HIGH);
    bpm -= 5;
    delay(500);
    // call function that increases the bpm
  }
  digitalWrite(decreaseLed, LOW);
  return bpm;
}

// INTERRUPT FUNCTION
void buttonPress() {
  if(digitalRead(measureButton) == HIGH) {
    measureState = HIGH;
  }
  else if(digitalRead(increaseButton) == HIGH ) {
    increaseState = HIGH;
  }
  else if(digitalRead(decreaseButton) == HIGH) {
    decreaseState = HIGH;
  }
}

bool debounce( bool newDebounceState ) {
  if( newDebounceState != lastDebounceState ) {
    lastButtonChangeTime = millis();
  }

  if( (millis() - lastButtonChangeTime) > debounceDuration ) {
    if( newDebounceState != currentDebounceState ) {
      currentDebounceState = newDebounceState;
    }
  }

  lastDebounceState = newDebounceState;

  return currentDebounceState;
}