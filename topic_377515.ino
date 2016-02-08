// Sketch for Jason187781
// http://forum.arduino.cc/index.php?topic=377515

// comment this out to remove the DEV mockup
#define DEV
// comment this out to remove the printing to serial
// #define PRINT

const int NBUTTONS = 3;

struct ButtonAndLed {
  const byte buttonPin;
  const byte ledPin;
  const int divisionFactor;
};

struct ButtonAndLed pinout[NBUTTONS] = {
  {2, 5, 1}, // full speed button anbd indicator led
  {3, 6, 2}, // half speed button anbd indicator led
  {4, 7, 4}  // quarter speed button anbd indicator led
};

const byte sensorPotPin = A0;
const byte controlPotPin = A1;

#ifdef DEV
////////////////////////////////////////////////////////
// THIS BLOCK IS FOR TESTING AND DEVELOPMENT
//
// I don't have the hardware to properly demo this, so these
// stub functions must be provided by my client Jason187781.
////////////////////////////////////////////////////////

// in the real sketch, this gets removed.
int test_dev_frame_location = 0;

int getSesnsorPotReading() {
  // in the real sketch, this will be replaced with
  // analogReead(sensorPotPin)
  return test_dev_frame_location;
}

void moveStepperUp() {
  // in the real sketch, this function must talk to the stepper controller
  Serial.print(" UP ");
  if (++test_dev_frame_location >= 1024) test_dev_frame_location = 1024;
}

void moveStepperDown() {
  Serial.print(" DN ");
  // in the real sketch, this function must talk to the stepper controller
  if (--test_dev_frame_location <= 0) test_dev_frame_location = 0;
}

////////////////////////////////////////////////////////
// END OF TEST/DEV BLOCK
////////////////////////////////////////////////////////
#endif 

int targetOffset = 0;
int currentSelection = 0;

void setup() {
#ifdef PRINT
  ///////////////// TEST/DEV //////////////
  Serial.begin(57600);
  while(!Serial);
#endif

  for (int i = 0; i < NBUTTONS; i++) {
    pinMode(pinout[i].buttonPin, INPUT_PULLUP);
    pinMode(pinout[i].ledPin, OUTPUT);
  }

  digitalWrite(pinout[currentSelection].ledPin, HIGH);
}

void loop() {
  const int sensor = getSesnsorPotReading();
  const int control = analogRead(controlPotPin);

  // I unconditionally read the buttons. this means that
  // while any button is being held down, nothing happens.
  // you can use this to recenter the control pot if you want
  for (int i = 0; i < NBUTTONS; i++) {
    if (digitalRead(pinout[i].buttonPin) == LOW) {
      digitalWrite(pinout[currentSelection].ledPin, LOW);
      currentSelection = i;
      digitalWrite(pinout[currentSelection].ledPin, HIGH);
      // pick values so that wherever the pots are right now
      // becomes the new target once the button is released
      targetOffset = sensor - (control / pinout[currentSelection].divisionFactor);
      return;
    }
  }

  int target = targetOffset + (control / pinout[currentSelection].divisionFactor);
  target = constrain(target, 0, 1023);

  // we permit a little slop to handle the fact that pots are a bit noisy
  if (target < sensor-1) moveStepperDown();
  else if (target > sensor+1) moveStepperUp();
  else /* do nothing */ ;
  

#ifdef PRINT  
  Serial.print(" selection ");
  Serial.print(currentSelection);
  Serial.print(" control ");
  Serial.print(control);
  Serial.print(" targetOffset ");
  Serial.print(targetOffset);
  Serial.print(" target ");
  Serial.print(target);
  Serial.print(" sensor ");
  Serial.print(sensor);
  Serial.println();
  delay(50);
#endif    
}
