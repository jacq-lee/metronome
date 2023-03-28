// LED AND SPEAKER PINS
const int noteSpeaker = 7;
const int measureLed = 8;
const int noteLed = 9;
const int increaseLed = 10;
const int decreaseLed = 11;

// BUTTON PINS
const int anyButton = 2; // Output of OR gate, used in the interrupt function
const int measureButton = 3;
const int noteButton = 4;
const int increaseButton = 5;
const int decreaseButton = 6;

// BUTTON STATES IN VOID LOOP
bool noteButtonState;
bool lastNoteButtonState;

// INTERRUPT VARIABLES
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
  // Used to toggle the time setting (Measure and Note buttons)
bool measuring = LOW;
bool noteMeasuring = HIGH;
  // Used for tracking the time between button presses
unsigned long startTime;
unsigned long timeElapsed;
unsigned long lastHigh; // Specifically used to avoid debounce/press and hold issues
int timeOut = 16000; // Device will exit measuring state if user does not press the measure button before this time

// METRONOME VARIABLES
int noteSpeed = 1000; // Default note speed
int timeSignature = 4; // Default time signature
int bpmChangeMultiplier = 1000; // Dictates how much bpm should increase/decrease by
int bpmChange; // Integer value of by how many seconds the speed increases/decreases
  // in bleep
unsigned long currentMillis;
unsigned long lastMillis;
int j = 0;
const int bleepDuration = 100; // How long the LEDs/speaker turn on for

// FUNCTIONS:
// void loop
  // Receives input from the: 
    // Measure button, then note button, to change the timing
    // Increase button to increase the bpm
    // Decrease button to decrease the bpm
    // Note button to reset the timing (press + hold for 4 seconds)
// void bleeping
  // Blinks LEDs/speaker in accordance to noteSpeed and timeSignature
// void buttonPress
  // interrupt function that is called when any of measureButton, increaseButton, or decreaseButton are called
  // digitalReads the inputs to tell which of the buttons was pressed, then sets corresponding flag high
// bool debounce
  // Prevents debounce issues when buttons are pressed (namely the noteButton for reset)


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
  pinMode(noteSpeaker, OUTPUT);
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

  // INTERRUPT FUNCTION CALL
  attachInterrupt(digitalPinToInterrupt(anyButton), buttonPress, RISING);
}


void loop() {
  // put your main code here, to run repeatedly:
  
  if( measureState == HIGH && (4*millis() - lastHigh > debounceDuration*2) ) {
    Serial.println("Setting Timing...");
    // Measuring state turns on
    lastHigh = 4*millis();   
    startTime = 4*millis(); // multiply by 4 to get proper time given 256 prescaler 
    digitalWrite(noteLed, HIGH);
    measuring = HIGH; // Measuring the new timing is toggled on/off
    noteMeasuring = HIGH;
    measureState = LOW;
    
    // While loop: in process of measuring, waiting for second measure button input
    while(measuring == HIGH) {
      digitalWrite(measureLed, HIGH);    
      noteButtonState = debounce(digitalRead(noteButton));
      if((noteButtonState == HIGH) && (lastNoteButtonState == LOW) && (noteMeasuring == HIGH) ) {
        noteSpeed = 4*millis() - startTime;
        Serial.print("Note Speed: ");
        Serial.print(noteSpeed);
        Serial.println(" (ms)");
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
        Serial.print(timeElapsed);
        Serial.println(" (ms)");
        Serial.print("Time Signature: ");
        Serial.print(timeSignature);
        Serial.println("/4");
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
        Serial.println("Timed Out!");
        Serial.println();
        digitalWrite(measureLed, LOW);
        digitalWrite(noteLed, LOW);
        measuring = LOW;
        measureState = LOW;
      }
    }
  }
  else if( measureState == HIGH ) { // measureButton was high from a previous button press
    measureState = LOW;
  }
  else if( (increaseState == HIGH) && (4*millis() - lastHigh > debounceDuration*2) ) {
    Serial.println("Increasing Note Speed...");
    
    // Increase state turns on
    measuring = HIGH; // Measuring the new timing is toggled on/off
    startTime = 4*millis(); // multiply by 4 to get proper time given 256 prescaler
    digitalWrite(increaseLed, HIGH);
    increaseState = LOW;
    lastHigh = 4*millis();
    
    while( measuring == HIGH ) {
      if( increaseState == HIGH ) { // Account for long press
        if( 4*millis() - lastHigh < debounceDuration*2 ) {
          increaseState = LOW;
        }
        else {
          lastHigh = 4*millis();
          timeElapsed = (4*millis() - startTime); 
          bpmChange = timeElapsed/bpmChangeMultiplier;
          noteSpeed = noteSpeed - bpmChange*1000;
          if( noteSpeed < 500 ) {
            noteSpeed = 500;
          }
          // Print time information to Serial Monitor
          Serial.print("Note Speed Increased By: ");
          Serial.println(bpmChange);
          Serial.print("New Note Speed: ");
          Serial.print(noteSpeed);
          Serial.println(" (ms)");
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
  else if( increaseState == HIGH ) { // increaseButton was high from a previous button press
    increaseState = LOW;
  }
  else if( (decreaseState == HIGH) && (4*millis() - lastHigh > debounceDuration*2) ) {
    Serial.println("Decreasing Note Speed...");
    
    // Increase state turns on
    measuring = HIGH; // Measuring the new timing is toggled on/off
    startTime = 4*millis(); // multiply by 4 to get proper time given 256 prescaler
    digitalWrite(decreaseLed, HIGH);
    decreaseState = LOW;
    lastHigh = 4*millis();
    
    while( measuring == HIGH ) {
      if( decreaseState == HIGH ) { // Account for long press
        if( 4*millis() - lastHigh < debounceDuration*2 ) {
          decreaseState = LOW;
        }
        else {
          lastHigh = 4*millis();
          timeElapsed = (4*millis() - startTime); // Measure button was pushed a second time, length between button presses is the speed that the metronome goes (in s).
          bpmChange = timeElapsed/bpmChangeMultiplier;
          noteSpeed = noteSpeed + bpmChange*1000;
          if( noteSpeed > 10000 ) {
            noteSpeed = 10000;
          }
          // Print time information to Serial Monitor
          Serial.print("Note Speed Decreased By: ");
          Serial.println(bpmChange);
          Serial.print("New Note Speed: ");
          Serial.print(noteSpeed);
          Serial.println(" (ms)");
          Serial.println();

          // Turn off indicator LEDs
          digitalWrite(decreaseLed, LOW);
          // Reset flags
          measuring = LOW;
          decreaseState = LOW;
        }
      }
    }
  }
  else if( decreaseState == HIGH ) { // decreaseButton was high from a previous button press
    decreaseState = LOW;
  }

  // RESET STATEMENT
  noteButtonState = debounce( (digitalRead(noteButton)) );
  if( noteButtonState == HIGH && lastNoteButtonState == LOW ) {
    Serial.println("Hold button for 4 seconds to reset...");
    startTime = 4*millis();
    while( noteButtonState == HIGH ) {
      bleeping(noteSpeed, timeSignature);
      while((noteButtonState == HIGH) && (4*millis() - startTime > 4000)) {
        digitalWrite(decreaseLed, HIGH);
        digitalWrite(measureLed, HIGH);
        digitalWrite(noteLed, HIGH);
        digitalWrite(increaseLed, HIGH);
        noteButtonState = debounce( (digitalRead(noteButton)) );
        if( noteButtonState == LOW ) {
          noteSpeed = 1000;
          timeSignature = 4;
          Serial.println("Reset!");
          Serial.print("Note Speed: ");
          Serial.print(noteSpeed);
          Serial.println(" (ms)");
          Serial.print("Time Signature: ");
          Serial.print(timeSignature);
          Serial.println("/4");
          Serial.println();
          digitalWrite(decreaseLed, LOW);
          digitalWrite(measureLed, LOW);
          digitalWrite(noteLed, LOW);
          digitalWrite(increaseLed, LOW);
        }
      }
      noteButtonState = debounce( (digitalRead(noteButton)) );
    }
  }
  lastNoteButtonState = noteButtonState;
  
  bleeping(noteSpeed, timeSignature);

}


// OUTPUT, BLINK LEDs AND SPEAKER
void bleeping(int noteSpeed, int timeSignature) {

  currentMillis = 4*millis();
  if( currentMillis - lastMillis >= noteSpeed ) {
    lastMillis = currentMillis;
    digitalWrite(noteLed, HIGH);
    digitalWrite(noteSpeaker, HIGH);
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
    digitalWrite(noteSpeaker, LOW);
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


// DEBOUNCE FUNCTION
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