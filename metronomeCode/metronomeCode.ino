// METRONOME TIMING VARIABLES
int bpm;

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
volatile byte measureState = LOW;
volatile byte increaseState = LOW;
volatile byte decreaseState = LOW;
volatile byte lastPress = LOW;

// DEBOUNCE VARIABLES
  // Button state variables used in debounce 
bool newDebounceState;
bool lastDebounceState;
bool currentDebounceState;
  // Timing variables used in debounce
unsigned long lastButtonChangeTime = 0;
unsigned long debounceDuration = 50;

// MEASURING VARIABLES
  // in loop
bool measuring = LOW;
bool noteMeasuring = HIGH;
bool lastMeasureState;
int startTime;
int timeElapsed;
int noteSpeed = 1000;
int bpmChange;
int lastHigh;
// in measurePress
int timeOut = 100000;
int quarterNote = 1000;
  // in bleep
int timeSignature = 4;
int currentMillis;
int lastMillis;
int j = 0;
const int bleepDuration = 100;

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



  if( measureState == HIGH && (4*millis() - lastHigh > 50) ) {
    Serial.println("Setting Timing...");
    // Measuring state turns on
    lastHigh = 4*millis();    
    measuring = HIGH; // Measuring the new timing is toggled on/off
    noteMeasuring = HIGH;
    startTime = 4*millis(); // multiply by 4 to get proper time given 256 prescaler
    digitalWrite(noteLed, HIGH);
    measureState = LOW;
    
    // While loop: in process of measuring, waiting for second measure button input
    while(measuring == HIGH) {
      digitalWrite(measureLed, HIGH);
      // noteSpeed = measurePress(startTime, noteSpeed); // Goes into while loop to wait for the note button to be pressed
      
      noteButtonState = debounce(digitalRead(noteButton));
      if((noteButtonState == HIGH) && (lastNoteButtonState == LOW) && (noteMeasuring == HIGH) ) {
        noteSpeed = 4*millis() - startTime;
        Serial.print("Note Speed: ");
        Serial.println(noteSpeed);
        digitalWrite(noteLed, LOW);
        noteMeasuring = LOW;
        measureState = LOW;
      }
      lastNoteButtonState = noteButtonState;

      // Measure button is pressed again to exit the measuring state
      if( measureState == HIGH && noteMeasuring == LOW ) {
        lastHigh = 4*millis();
        timeElapsed = (4*millis() - startTime); // Measure button was pushed a second time, length between button presses is the speed that the metronome goes (in s).
        timeSignature = timeElapsed/noteSpeed;
        // Print time information to Serial Monitor
        Serial.print("Time Elapsed: ");
        Serial.println(timeElapsed);
        Serial.print("Time Signature: ");
        Serial.println(timeSignature);
        Serial.println();
        // Turn off indicator LEDs
        digitalWrite(measureLed, LOW);
        digitalWrite(noteLed, LOW);
        // Reset flags
        measuring = LOW;
        measureState = LOW;
      }
      // Time out and exit the statement by setting measuring to LOW
      else if( 4*millis() - startTime > timeOut ) {
        digitalWrite(measureLed, LOW);
        digitalWrite(noteLed, LOW);
        measuring = LOW;
        measureState = LOW;
      }
    }
  }
  else if( measureState == HIGH ) {
    measureState = LOW;
  
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
    // measureState = LOW; // Reset the flag so user can exit measureState
  }
  else if( (increaseState == HIGH) && (4*millis() - lastHigh > 50) ) {
    Serial.println('Increasing...');
    
    // Increase state turns on
    lastHigh = 4*millis();    
    measuring = HIGH; // Measuring the new timing is toggled on/off
    startTime = 4*millis(); // multiply by 4 to get proper time given 256 prescaler
    digitalWrite(increaseLed, HIGH);
    increaseState = LOW;
    
    while( measuring == HIGH ) {
      if( increaseState == HIGH ) {
        if( 4*millis() - lastHigh < 50 ) {
          increaseState = LOW;
        }
        else {
          lastHigh = 4*millis();
          timeElapsed = (4*millis() - startTime); // Measure button was pushed a second time, length between button presses is the speed that the metronome goes (in s).
          bpmChange = timeElapsed/1000;
          noteSpeed = noteSpeed + bpmChange;
          // Print time information to Serial Monitor
          Serial.print("BPM Increased By: ");
          Serial.println(bpmChange);
          Serial.print("New Note Speed: ");
          Serial.println(noteSpeed);
          Serial.println();

          // Turn off indicator LEDs
          digitalWrite(increaseLed, LOW);
          // Reset flags
          measuring = LOW;
          increaseState = LOW;
        }
      }
    }
  }
  // else if( decreaseState == HIGH ) {
  //   decreaseState = LOW;
  // }

  bleeping(noteSpeed, timeSignature);
  
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


int measurePress(int initialTime, int previousQuarterNote) {
  // Sets quarter note time 

  // MEASURE BUTTON HAS BEEN PRESSED, WAIT FOR NOTE BUTTON TO BE PRESSED
  noteButtonState = debounce(digitalRead(noteButton));
  while( noteButtonState == LOW ) {
    if( (4*millis() - initialTime ) > timeOut ) {
      return previousQuarterNote;
    }
    else { 
      noteButtonState = debounce(digitalRead(noteButton));
      if( (noteButtonState == HIGH) && (lastNoteButtonState == LOW) ) {
        quarterNote = 4*millis() - initialTime;
        return quarterNote;
      }
      lastNoteButtonState = noteButtonState;
   }
  }  

  return quarterNote;
  // Look for timing
  // if( currentlyMeasuring == HIGH ) {
  //   digitalWrite(measureLed, HIGH);
  //   startTime = 4*millis(); // multiply by 4 to get proper time given 256 prescaler
  // }

  // if( currentlyMeasuring == LOW ) {
  //   if(debounce(measureButton) == HIGH) {
  //     int endTime = 4*millis(); // button is pushed a second time
  //     if (((endTime - startTime)/1000) < 5) {     // to change: must press measurebutton 2nd time after X seconds
  //       int newSpeed = ((endTime - startTime)/1000);   // length between button presses is the speed that the metronome goes (in s). 
  //       // call function to change metronome speed
  //     }
  //   }
  //   // for resetting
  //   while (debounce(measureButton) == HIGH) {
  //   if(debounce(measureButton) == LOW) {
  //     int endTime = 4*millis()
  //       if (((startTime - endTime)/1000) >= 3) { // if held for 3 seconds
  //           bpm = 100; // reset metronome to base speed
  //           // call function to change metronome speed
  //     }
  //     }
  //   }
  //   digitalWrite(measureLed, LOW);

  // }
}


// OUTPUT 
void bleeping(int noteSpeed, int timeSignature) {

  currentMillis = 4*millis();
  if( currentMillis - lastMillis >= noteSpeed ) {
    lastMillis = currentMillis;
    digitalWrite(noteLed, HIGH);
    j++;
    if( j >= timeSignature ) {
      // Serial.print(currentMillis);
      // Serial.println(j);
      digitalWrite(measureLed, HIGH);
      j = 0;
    }
  }
  if( currentMillis - lastMillis >= bleepDuration ) {
    digitalWrite(noteLed, LOW);
    digitalWrite(measureLed, LOW);
  }
}



// INTERRUPT FUNCTION
void buttonPress() {
  if( (4*millis() - lastPress) > debounceDuration ) { 
    if(digitalRead(measureButton)) {
      measureState = HIGH;
    }
    else if(digitalRead(increaseButton) == HIGH ) {
      increaseState = HIGH;
    }
    else if(digitalRead(decreaseButton) == HIGH) {
      decreaseState = HIGH;
    }  
  }
  lastPress = 4*millis();
}

bool debounce( bool newDebounceState ) {
  if( newDebounceState != lastDebounceState ) {
    lastButtonChangeTime = 4*millis();
  }

  if( (4*millis() - lastButtonChangeTime) > debounceDuration ) {
    if( newDebounceState != currentDebounceState ) {
      currentDebounceState = newDebounceState;
    }
  }

  lastDebounceState = newDebounceState;

  return currentDebounceState;
}