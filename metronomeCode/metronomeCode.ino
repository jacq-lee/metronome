// METRONOME TIMING VARIABLES
int bpm;
// int initialTime;
int stopTime;

// LED AND SPEAKER PINS
// const int metronomeSpeaker = 7;
const int measureLed = 8;
const int noteLed = 9;
const int increaseLed = 10;
const int decreaseLed = 11;

// LED STATES
int measureLedState = LOW;
int noteLedState = LOW;
int increaseLedState = LOW;
int decreaseLedState = LOW;

// BUTTON PINS
const int measureButton = 3;
const int noteButton = 4;
const int increaseButton = 5;
const int decreaseButton = 6;

// BUTTON STATES IN VOID LOOP
bool measureButtonState;
bool lastMeasureButtonState;
bool noteButtonState;
bool lastNoteButtonState;
bool increaseButtonState;
bool lastIncreaseButtonState;
bool decreaseButtonState;
bool lastDecreaseButtonState;

// INTERRUPT VARIABLES
const int anyButton = 2;
volatile byte measureState;
volatile byte increaseState;
volatile byte decreaseState;
volatile byte lastPress;

// DEBOUNCE VARIABLES
  // Button state variables used in debounce 
bool newDebounceState;
bool lastDebounceState;
bool currentDebounceState;
  // Timing variables used in debounce
unsigned long lastButtonChangeTime = 0;
unsigned long debounceDuration = 5;

// MEASURING VARIABLES
bool measuring;
int startTime;
int endTime;
int timeOut;
int quarterNote;
const int bleepDuration = 200;

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
  Serial.begin(9600);
  
  // put your setup code here, to run once:
  // BUTTON PINS
  pinMode(anyButton, INPUT);
  pinMode(measureButton, INPUT);
  pinMode(noteButton, INPUT);
  pinMode(increaseButton, INPUT);
  pinMode(decreaseButton, INPUT);

  // LED AND SPEAKER PINS
  // pinMode(metronomeSpeaker, OUTPUT);
  pinMode(measureLed, OUTPUT);
  pinMode(noteLed, OUTPUT);
  pinMode(increaseLed, OUTPUT);
  pinMode(decreaseLed, OUTPUT);

  // SET UP TIMER0
  TCCR0A = 0; // set register to 0, TCCR0A controls PWM
  TCCR0B = 0; // set register to 0, TCCR0B controls the timer itself
  TCCR0B |= (1 << CS02); // 256 prescaler
  TCCR1B = (1 << WGM01); // turns on CTC mode
  TCNT0 = 0; // initialize counter value to 0s // COUNTER ACCESS

  // SET UP TIMER1
  // TCCR1A = 0; // initial value
  // TCCR1B = 0; // initial value
  // TCCR1B |= (1 << CS12); // prescaler for 256
  // TCNT1 = timer; // preload timer 

  // INTERRUPT FUNCTION CALL
  attachInterrupt(digitalPinToInterrupt(anyButton), buttonPress, RISING);
}


void loop() {
  // put your main code here, to run repeatedly:
  /*
  measureButtonState = debounce( digitalRead(measureButton) );
    
  if(measureButtonState == HIGH && lastMeasureButtonState == LOW) {
    measureLedState = !measureLedState;
    digitalWrite(measureLed, measureLedState);
    // initialTime = TCNT0/256;
    Serial.println(TCNT0);

    // increaseButtonState = debounce(digitalRead(increaseButton));
    // if( increaseButtonState == HIGH && lastIncreaseButtonState == LOW) {
    //   stopTime = TCNT0;
    //   Serial.println(stopTime);
    // }
    // lastIncreaseButtonState = increaseButtonState;
  }
  lastMeasureButtonState = measureButtonState;
  */

  bleeping(speed);

  if( measureState == HIGH ) {
    measuring != measuring;
    // could put the below if/else in a function
    if( measuring == HIGH ) { // This means we were previously not measuring and will now start measuring;
      
      digitalWrite(measureLed, HIGH);
      startTime = 4*millis(); // multiply by 4 to get proper time given 256 prescaler

      measurePress(startTime);
    }
    else { // Measuring was high - we were measuring already and should now stop and apply the new timing
      endTime = 4*millis(); // button is pushed a second time
      newSpeed = ((endTime - startTime)/1000); // length between button presses is the speed that the metronome goes (in s). 
      digitalWrite(measureLed, LOW);
      // call function to change metronome speed
    }
    
    
    /* COME BACK TO---------------------------------------------------------------------
    // for resetting
    while (debounce(measureButton) == HIGH) {
    if(debounce(measureButton) == LOW) {
      int endTime = 4*millis()
        if (((startTime - endTime)/1000) >= 3) { // if held for 3 seconds
            bpm = 100; // reset metronome to base speed
            // call function to change metronome speed
      }
    }
    -------------------------------------------------------------------------------------*/
    measureState = LOW; // Reset the flag so user can exit measureState
  }
  else if( increaseState == HIGH ) {
    d
  }
  else if( decreaseState == HIGH ) {
    
  }

  while( measuring == HIGH ) {
    s
  }

  if( (measuring == HIGH) && (timeElapsed > timeOut) ) { 
    // do the CLC thing
    measuring = LOW;
  }
  
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


void measurePress(int initialTime) {
  
  // sets quarter note 
  // MEASURE BUTTON HAS BEEN PRESSED, WAIT FOR NOTE BUTTON TO BE PRESSED
  while( noteButtonState == LOW ) {
    if( (millis() - initialTime ) > timeOut ) {
      // LEAVE FUNCTION, TIME OUT, GO BACK TO ORIGINAL
    }
   else { 
      noteButtonState = debounce(digitalRead(noteButton));
      if( (noteButtonState == HIGH) && (lastNoteButtonState == LOW) ) {
        quarterNote = 4*millis() - startTime;
      }
      lastNoteButtonState = noteButtonState;
      // call function to change note length
   }
  }  


  // Look for timing
  if( currentlyMeasuring == HIGH ) {
    digitalWrite(measureLed, HIGH);
    startTime = 4*millis(); // multiply by 4 to get proper time given 256 prescaler
  }

  if( currentlyMeasuring == LOW ) {
    if(debounce(measureButton) == HIGH) {
      int endTime = 4*millis(); // button is pushed a second time
      if (((endTime - startTime)/1000) < 5) {     // to change: must press measurebutton 2nd time after X seconds
        int newSpeed = ((endTime - startTime)/1000);   // length between button presses is the speed that the metronome goes (in s). 
        // call function to change metronome speed
      }
    }
    // for resetting
    while (debounce(measureButton) == HIGH) {
    if(debounce(measureButton) == LOW) {
      int endTime = 4*millis()
        if (((startTime - endTime)/1000) >= 3) { // if held for 3 seconds
            bpm = 100; // reset metronome to base speed
            // call function to change metronome speed
      }
      }
    }
    digitalWrite(measureLed, LOW);

  }
}


// OUTPUT 
void bleeping(int measureSpeed, int noteSpeed) {

  if( millis()*4 %  )
  digitalWrite
}



// INTERRUPT FUNCTION
void buttonPress() {
  if( (millis() - lastPress) > debounceDuration ) { 
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
  lastPress = millis();
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