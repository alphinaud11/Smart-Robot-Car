#include <Arduino_FreeRTOS.h>
#include <AFMotor.h>
#include <Servo.h>
#include <Wire.h>
#include <TEA5767N.h>

// Back Sensor Pins
#define trigBack 22
#define echoBack 24
// Side Sensor Pins
#define trigSide 23
#define echoSide 25
// Buzzer Pin
#define buzzer 27
// Servo Pin
#define servoPin 45
// Rain Drop Pin
#define rainDrop A8
// Water Level Pin
#define waterLevel A9
// 7 Segment Pins
#define g 29
#define f 31
#define a 33
#define b 35
#define e 37
#define d 39
#define c 41
 
AF_DCMotor motor_1(1, MOTOR12_64KHZ); // create motor #1, 64KHz pwm
AF_DCMotor motor_2(2, MOTOR12_64KHZ); // create motor #2, 64KHz pwm
AF_DCMotor motor_3(3, MOTOR12_64KHZ); // create motor #3, 64KHz pwm
AF_DCMotor motor_4(4, MOTOR12_64KHZ); // create motor #4, 64KHz pwm
Servo servo;
bool parked = false;
long durationBack;
int distanceBack;
long durationSide;
int distanceSide;
int rainValue;
int levelValue;
TEA5767N radio = TEA5767N();
double stations[] = {100.6, 104.2,90.90};
int Rxcommand = 0;
TaskHandle_t park_Handle;

// Functions
void buzzFar() {
  digitalWrite(buzzer, HIGH);
  vTaskDelay(pdMS_TO_TICKS(500));
  //delay(500);
  digitalWrite(buzzer, LOW);
  vTaskDelay(pdMS_TO_TICKS(500));
  //delay(500);
}

void buzzNear() {
  digitalWrite(buzzer, HIGH);
  vTaskDelay(pdMS_TO_TICKS(100));
  //delay(100);
  digitalWrite(buzzer, LOW);
  vTaskDelay(pdMS_TO_TICKS(100));
  //delay(100);
}

void readBack() {
  delay(100);
  digitalWrite(trigBack, LOW);
  delayMicroseconds(2);

  digitalWrite(trigBack, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigBack, LOW);

  durationBack = pulseIn(echoBack, HIGH);
  distanceBack = durationBack * 0.034/2;
}

void readSide() {
  delay(100);
  digitalWrite(trigSide, LOW);
  delayMicroseconds(2);

  digitalWrite(trigSide, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigSide, LOW);

  durationSide = pulseIn(echoSide, HIGH);
  distanceSide = durationSide * 0.034/2;
}

void detectSpace() {
  bool spaceFound = false;
  while (!spaceFound) {
    readSide();
    if (distanceSide >= 20) {
      spaceFound = true;
    }  
  }
}

void detectSpot() {
  bool spotFound = false;
  while (!spotFound) {
    readSide();
    if (distanceSide < 20) {
      stopCar();
      spotFound = true;
    }  
  }
}

void detectObstacle() {
  bool obstacleFound = false;
  while (!obstacleFound) {
    readBack();
    if (distanceBack <= 15) {
      stopCar();
      obstacleFound = true;
    }  
  }
}

void zero() {
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(g, LOW);
}

void one() {
  digitalWrite(a, LOW);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, LOW);
  digitalWrite(e, LOW);
  digitalWrite(f, LOW);
  digitalWrite(g, LOW);
}

void two() {
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, LOW);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(f, LOW);
  digitalWrite(g, HIGH);
}

void checkRain() {
  rainValue = analogRead(rainDrop);
  if (rainValue < 800) {
    servo.write(180);
    delay(1000);
    servo.write(0);
    delay(1000);
    }
    delay(200);
}

void checkFuel() {
  levelValue = analogRead(waterLevel);
  if (levelValue >= 0 && levelValue < 610) {
    zero();
  }
  else if (levelValue >= 610 && levelValue < 630) {
    one(); 
  }
  else if (levelValue >= 630) {
    two();
  }  
  delay(200);
}

void checkRadio() {
  Wire.requestFrom(5,1);
  int coming = Wire.read();
  if (coming != Rxcommand) {
    Rxcommand = coming;
    if (Rxcommand >= 0 && Rxcommand <= 2) {
    radio.selectFrequency(stations[Rxcommand]);
  }
  else if (Rxcommand == 3) {
    radio.mute();
  }
  else {
    radio.turnTheSoundBackOn();
  } 
  }
  delay(200);
}

void setSpeed1(int value) {
  motor_1.setSpeed(value);     // set the speed to 80/255
  motor_2.setSpeed(value);     // set the speed to 80/255
  motor_3.setSpeed(value);     // set the speed to 80/255
  motor_4.setSpeed(value);     // set the speed to 80/255
}

void moveForward() {
  motor_1.run(FORWARD);
  motor_2.run(FORWARD);
  motor_3.run(FORWARD);
  motor_4.run(FORWARD);
}

void moveBackward() {
  motor_1.run(BACKWARD);
  motor_2.run(BACKWARD);
  motor_3.run(BACKWARD);
  motor_4.run(BACKWARD);
}

void moveLeft() {
  motor_1.run(BACKWARD);
  motor_2.run(BACKWARD);
  motor_3.run(FORWARD);
  motor_4.run(FORWARD);
}

void moveRight() {
  motor_1.run(FORWARD);
  motor_2.run(FORWARD);
  motor_3.run(BACKWARD);
  motor_4.run(BACKWARD);
}

void stopCar() {
  motor_1.run(RELEASE);
  motor_2.run(RELEASE);
  motor_3.run(RELEASE);
  motor_4.run(RELEASE);
}

void park(void* pvParameters) {
  while(1) {
    if (!parked) {
      moveForward();
    detectSpace();
    vTaskDelay(pdMS_TO_TICKS(100));
    detectSpot();
    for (int i=0; i<4; i++) {
      buzzFar();
    }
    moveBackward();
    vTaskDelay(pdMS_TO_TICKS(700));
    stopCar();
    vTaskDelay(pdMS_TO_TICKS(3000));
    setSpeed1(100);
    moveLeft();
    vTaskDelay(pdMS_TO_TICKS(800));
    stopCar();
    vTaskDelay(pdMS_TO_TICKS(1000));
    setSpeed1(80);
    moveBackward();
    detectObstacle();
    for (int i=0; i<20; i++) {
      buzzNear();
    }
    setSpeed1(110);
    moveRight();
    vTaskDelay(pdMS_TO_TICKS(1000));
    stopCar();
    vTaskDelete(park_Handle);
    parked = true; 
    }
  }
}

void task2(void* pvParameters) {
  while(1) {
    checkRadio();
    checkRain();
    checkFuel();
  }
}
 
void setup() {
  //Wire.begin();
  
  motor_1.setSpeed(80);     // set the speed to 80/255
  motor_2.setSpeed(80);     // set the speed to 80/255
  motor_3.setSpeed(80);     // set the speed to 80/255
  motor_4.setSpeed(80);     // set the speed to 80/255
  pinMode(trigBack, OUTPUT);
  pinMode(echoBack, INPUT);
  pinMode(trigSide, OUTPUT);
  pinMode(echoSide, INPUT);
  pinMode(buzzer, OUTPUT);
  servo.attach(servoPin);
  servo.write(0);
  zero();
  
  xTaskCreate(park, "Parking task", 1000, NULL, 2, &park_Handle);
  xTaskCreate(task2, "Other features", 1000, NULL, 1, NULL);
}

void loop(){
  
}
