const int motor1pin1 = 10;
const int motor1pin2 = 9;
const int motor1PWM  = 6;

const int motor2pin1 = 3;
const int motor2pin2 = 5;
const int motor2PWM  = 11;

const int touchPin = 13;
bool robotEnabled = false;
int lastTouchState = LOW;


bool checkTouch() {
  int currentTouchState = digitalRead(touchPin);
  bool toggled = false;

  if (currentTouchState == HIGH && lastTouchState == LOW) {
    robotEnabled = !robotEnabled;
    toggled = true;
    if (!robotEnabled) stop();
    Serial.println(robotEnabled ? "Robot ON" : "Robot OFF");
    delay(50); // debounce
  }

  lastTouchState = currentTouchState;
  return toggled && !robotEnabled; // true = just turned off
}



void setup() {
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor1PWM,  OUTPUT);

  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);
  pinMode(motor2PWM,  OUTPUT);

  pinMode(touchPin, INPUT);
}

void moveForward() {
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  analogWrite(motor1PWM, 200);

  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
  analogWrite(motor2PWM, 200);
}

void stopMotors() {
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, LOW);
  analogWrite(motor1PWM, 0);

  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, LOW);
  analogWrite(motor2PWM, 0);
}

void loop() {
  
  if (!robotEnabled) return;
  moveForward();
}
