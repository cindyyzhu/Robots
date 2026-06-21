const int motor2pin1 = 10;
const int motor2pin2 = 9;
const int motor1PWM  = 6;   // ENA – set once to 200, then ignored

const int motor1pin1 = 3;
const int motor1pin2 = 5;
const int motor2PWM  = 11;  // ENB – set once to 200, then ignored

// ── Ultrasonic pins ──────────────────────────────────────────
const int trigFront = 4;
const int echoFront = 7;
const int trigSide  = 8;
const int echoSide  = 12;

// ── Touch sensor ─────────────────────────────────────────────
const int touchPin = 13;


//Touch
bool robotEnabled = false;
int lastTouchState = LOW;

const int minFrontDistance = 7;

void forward(){
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin2, LOW);
}

void turnRight(){
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, LOW);
}

void turnLeft(){
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
}

void stop(){
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, LOW);
}

float getDistance(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duration = pulseIn(echo, HIGH, 30000);
  if (duration == 0) return -1;
  return (duration * 0.0343) / 2.0;
}

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
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);

  pinMode(trigFront, OUTPUT);
  pinMode(echoFront, INPUT);
  pinMode(trigSide, OUTPUT);
  pinMode(echoSide, INPUT);

  pinMode(touchPin, INPUT);

  Serial.begin(9600);
  stop();
  Serial.println("Robot OFF — touch to enable");

  forward();
  delay(1000);
  turnRight();
  delay(1000);
  turnLeft();
  delay(1000);
  stop();

}

void loop() {
  checkTouch();

  //if (!robotEnabled) return;

  float frontDistance = getDistance(trigFront, echoFront);
  float sideDistance = getDistance(trigSide, echoSide);

  Serial.print("Front Distance: "); Serial.print(frontDistance); Serial.print(" cm");
  Serial.print(" Side Distance:  "); Serial.print(sideDistance); Serial.print(" cm ");

  if(frontDistance > minFrontDistance || frontDistance < 0){
    if(sideDistance < 5){
      delay(50);
      turnLeft();
      delay(100);
      forward();
      delay(200);
      Serial.println("too close to wall");
    }
    else if(sideDistance > 10){
      delay(50);
      turnRight();
      delay(100);
      forward();
      delay(200);
      Serial.println("too far from wall");
    }
    else{
      forward();
      Serial.println("forward");
    }
  }
  if(frontDistance <= minFrontDistance && frontDistance>0){
    turnLeft();
    delay(100);
    Serial.println("looking for wall");
  }

}
