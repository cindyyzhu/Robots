// ── Motor pins ───────────────────────────────────────────────
const int motor1pin1 = 4;
const int motor1pin2 = 5;
const int motor1PWM  = 9;
const int motor2pin1 = 6;
const int motor2pin2 = 7;
const int motor2PWM  = 10;

// ── Ultrasonic pins ──────────────────────────────────────────
const int trigFront = 11;
const int echoFront = 12;
const int trigRight = 2;
const int echoRight = 3;

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

// ── States ───────────────────────────────────────────────────
enum State { STOPPED, FORWARD, REVERSE_BEFORE_TURN, TURN_RIGHT, TURN_LEFT, UTURN, CLEAR_CORNER };
State state       = STOPPED;
State nextState   = FORWARD;
State pendingTurn = TURN_LEFT;

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

// ── Sensor ───────────────────────────────────────────────────
float getDistance(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH, 30000);
  if (duration == 0) return -1.0;
  return (duration * 0.0343) / 2.0;
}

// ── Touch ────────────────────────────────────────────────────
void checkTouch() {
  int cur = digitalRead(touchPin);
  if (cur == HIGH && lastTouchState == LOW) {
    robotEnabled = !robotEnabled;
    delay(50);
    if (robotEnabled) setState(FORWARD);
    else              setState(STOPPED);
    Serial.println(robotEnabled ? "Robot ON" : "Robot OFF");
  }
  lastTouchState = cur;
}

// ── State helpers ────────────────────────────────────────────
void setState(State s) {
  state = s;
  stateStartTime = millis();
  switch (s) {
    case STOPPED:             motorStop();       break;
    case FORWARD:             motorForward();    break;
    case REVERSE_BEFORE_TURN: motorReverse();    break;
    case TURN_RIGHT:          motorTurnRight();  break;
    case TURN_LEFT:           motorTurnLeft();   break;
    case UTURN:               motorTurnLeft();   break;
    case CLEAR_CORNER:        motorForward();    break;
  }
}

unsigned long msInState() { return millis() - stateStartTime; }

// ── Setup ────────────────────────────────────────────────────
void setup() {
  pinMode(motor1pin1, OUTPUT); pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT); pinMode(motor2pin2, OUTPUT);
  pinMode(motor1PWM,  OUTPUT); pinMode(motor2PWM,  OUTPUT);

  pinMode(trigFront, OUTPUT); pinMode(echoFront, INPUT);
  pinMode(trigRight, OUTPUT); pinMode(echoRight, INPUT);
  pinMode(touchPin,  INPUT);

  Serial.begin(9600);
  motorStop();
  Serial.println("Robot OFF — touch to enable");
}

// ── Loop ─────────────────────────────────────────────────────
void loop() {
  checkTouch();
  if (!robotEnabled) return;

  float front = getDistance(trigFront, echoFront);
  float right  = getDistance(trigRight,  echoRight);

  bool frontBlocked  = (front > 0 && front <= FRONT_CLEAR);
  bool rightOpen     = (right < 0  || right > SIDE_MAX);
  bool rightTooClose = (right > 0  && right < SIDE_MIN);
  bool rightTooFar   = (right > 0  && right > SIDE_TARGET_HIGH && right <= SIDE_MAX);

  Serial.print("State:"); Serial.print(state);
  Serial.print(" F:"); Serial.print(front);
  Serial.print(" R:"); Serial.println(right);

  switch (state) {

    case FORWARD:
      if (rightOpen && !frontBlocked) {
        pendingTurn = TURN_RIGHT;
        setState(REVERSE_BEFORE_TURN);
      } else if (frontBlocked && rightOpen) {
        pendingTurn = TURN_RIGHT;
        setState(REVERSE_BEFORE_TURN);
      } else if (frontBlocked && !rightOpen) {
        pendingTurn = TURN_LEFT;
        setState(REVERSE_BEFORE_TURN);
      } else {
        if (rightTooClose)    motorTurnLeft();
        else if (rightTooFar) motorTurnRight();
        else                  motorForward();
      }
      break;

    case REVERSE_BEFORE_TURN:
      if (msInState() >= REVERSE_MS) {
        setState(pendingTurn);
      }
      break;

    case TURN_RIGHT:
      if (msInState() >= TURN_90_MS) {
        nextState = FORWARD;
        setState(CLEAR_CORNER);
      }
      break;

    case TURN_LEFT:
      if (msInState() >= TURN_90_MS) {
        float newFront = getDistance(trigFront, echoFront);
        if (newFront > 0 && newFront <= FRONT_CLEAR) {
          setState(UTURN);
        } else {
          nextState = FORWARD;
          setState(CLEAR_CORNER);
        }
      }
      break;

    case UTURN:
      if (msInState() >= TURN_180_MS) {
        float newFront = getDistance(trigFront, echoFront);
        if (newFront < 0 || newFront > FRONT_CLEAR) {
          nextState = FORWARD;
          setState(CLEAR_CORNER);
        }
      }
      break;

    case CLEAR_CORNER:
      if (msInState() >= CLEAR_MS) {
        setState(nextState);
      }
      break;

    case STOPPED:
      break;
  }
}
