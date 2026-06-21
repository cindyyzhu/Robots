// ── Motor pins ───────────────────────────────────────────────
const int motor1pin1 = 10;
const int motor1pin2 = 9;
const int motor1PWM  = 6;

const int motor2pin1 = 3;
const int motor2pin2 = 5;
const int motor2PWM  = 11;



// ── Touch sensor ─────────────────────────────────────────────
const int touchPin = 13;

// ── Speed ────────────────────────────────────────────────────
const int FULL_SPEED = 200;
const int TURN_OUTER = 200;
const int TURN_INNER = 80;

// ── Tunable parameters ───────────────────────────────────────
const float FRONT_CLEAR         = 30.0;
const float SIDE_MIN            = 10.0;
const float SIDE_TARGET_HIGH    = 18.0;
const float SIDE_MAX            = 30.0;
const unsigned long REVERSE_MS  = 300;
const unsigned long TURN_90_MS  = 1200;
const unsigned long TURN_180_MS = 2400;
const unsigned long CLEAR_MS    = 500;


bool robotEnabled   = false;
int  lastTouchState = LOW;
unsigned long stateStartTime = 0;

// ── Motor primitives ─────────────────────────────────────────
void motorStop() {
  digitalWrite(motor1pin1, LOW); digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, LOW); digitalWrite(motor2pin2, LOW);
  analogWrite(motor1PWM, 0);
  analogWrite(motor2PWM, 0);
}

void motorForward() {
  digitalWrite(motor1pin1, HIGH); digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, HIGH); digitalWrite(motor2pin2, LOW);
  analogWrite(motor1PWM, FULL_SPEED);
  analogWrite(motor2PWM, FULL_SPEED);
}

void motorReverse() {
  digitalWrite(motor1pin1, LOW); digitalWrite(motor1pin2, HIGH);
  digitalWrite(motor2pin1, LOW); digitalWrite(motor2pin2, HIGH);
  analogWrite(motor1PWM, FULL_SPEED);
  analogWrite(motor2PWM, FULL_SPEED);
}

void motorTurnRight() {
  digitalWrite(motor1pin1, HIGH); digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, HIGH); digitalWrite(motor2pin2, LOW);
  analogWrite(motor1PWM, TURN_OUTER);
  analogWrite(motor2PWM, TURN_INNER);
}

void motorTurnLeft() {
  digitalWrite(motor1pin1, HIGH); digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, HIGH); digitalWrite(motor2pin2, LOW);
  analogWrite(motor1PWM, TURN_INNER);
  analogWrite(motor2PWM, TURN_OUTER);
}


// ── Touch ────────────────────────────────────────────────────
void checkTouch() {
  int cur = digitalRead(touchPin);
  if (cur == HIGH && lastTouchState == LOW) {
    robotEnabled = !robotEnabled;
    delay(50);
    if (robotEnabled) motorForward();
    else              motorStop();
    Serial.println(robotEnabled ? "Robot ON" : "Robot OFF");
  }
  lastTouchState = cur;
}



unsigned long msInState() { return millis() - stateStartTime; }

// ── Setup ────────────────────────────────────────────────────
void setup() {
  pinMode(motor1pin1, OUTPUT); pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT); pinMode(motor2pin2, OUTPUT);
  pinMode(motor1PWM,  OUTPUT); pinMode(motor2PWM,  OUTPUT);

  pinMode(touchPin,  INPUT);

  Serial.begin(9600);
  motorStop();
  Serial.println("Robot OFF — touch to enable");
}

// ── Loop ─────────────────────────────────────────────────────
void loop() {
  checkTouch();
  if (!robotEnabled) return;

  motorForward();


    
}
