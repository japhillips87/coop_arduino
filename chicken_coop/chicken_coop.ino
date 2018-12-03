const int photoresistor = A0;
const int enableMotor = 2;
const int motorIn1 = 3;
const int motorIn2 = 4;
const int reedOpen = 5;
const int reedShut = 6;
const int nightLightLevel = 40;
const int dayLightLevel = 80;
const int debounceDelay = 50;
const long loopDelay = (1000*60*10);
const long doorTimeout = 120000;

int reedShutState;
int reedOpenState;
bool somethingIsWrong = false;

void setup() {
  pinMode(enableMotor, OUTPUT);
  pinMode(motorIn1, OUTPUT);
  pinMode(motorIn2, OUTPUT);
  pinMode(photoresistor, INPUT);
  pinMode(reedShut, INPUT);
  pinMode(reedOpen, INPUT);

  reedShutState = digitalRead(reedShut);
  reedOpenState = digitalRead(reedOpen);
}

void loop() {
  if (somethingIsWrong) {
    return;
  }

  int lightLevel = getLightLevel();

  processLight(lightLevel);
  delay(loopDelay);
}

int getLightLevel() {
  return analogRead(photoresistor);
}

void processLight(int lightLevel) {
  if (lightLevel < nightLightLevel && !doorIsShut()) {
    Serial.println("Shutting Door");
    shutDoor();
  } else if (lightLevel > dayLightLevel && !doorIsOpen()) {
    Serial.println("Opening door");
    openDoor();
  }
}

void shutDoor() {
  setDoorToShutDirection();
  long shuttingTime = millis();

  while (!doorIsShut()) {
    if (millis() - shuttingTime > doorTimeout) {
      somethingIsWrong = true;
      break;
    }
    digitalWrite(enableMotor, HIGH);
  }
  stopMotor();
}

void openDoor() {
  setDoorToOpenDirection();
  long openingTime = millis();

  while (!doorIsOpen()) {
    if (millis() - openingTime > doorTimeout) {
      somethingIsWrong = true;
      break;
    }
    digitalWrite(enableMotor, HIGH);
  }
  stopMotor();
}

void setDoorToShutDirection() {
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, HIGH);
}

void setDoorToOpenDirection() {
  digitalWrite(motorIn1, HIGH);
  digitalWrite(motorIn2, LOW);
}

void stopMotor() {
  digitalWrite(enableMotor, LOW);
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, LOW);
}

boolean doorIsOpen() {
  reedOpenState = debounce(reedOpenState, reedOpen);

  return reedOpenState == HIGH;
}

boolean doorIsShut() {
  reedShutState = debounce(reedShutState, reedShut);

  return reedShutState == HIGH;
}

int debounce(int lastState, int input) {
  int reading = digitalRead(input);

  if (reading != lastState) {
    delay(debounceDelay);
    return digitalRead(input);
  }
  return lastState;
}
