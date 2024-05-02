#include <AccelStepper.h>

//Motor
#define dirPin1 5
#define stepPin1 4

int EN_PIN = 3;

#define Endstop 11

#define motorInterfaceType 1
AccelStepper rotation = AccelStepper(motorInterfaceType, stepPin1, dirPin1);


#define BUFFER_SIZE 16 // Set the buffer size based on the size of 4 integers (4 bytes each)
byte buffer[BUFFER_SIZE]; // Create a buffer to store incoming bytes

//const long stepsPerRevolution = 16200;
//const long fullTiltSteps = 2450;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  //Endstop Setup
//  pinMode(Endstop, INPUT);

  //Motor Set Up
  pinMode(EN_PIN, OUTPUT);
  digitalWrite (EN_PIN, LOW);
  rotation.setMaxSpeed(3000);
  rotation.setAcceleration(2000);

}

void loop() {  
  int potValue = analogRead(A0);

  // Map the potentiometer value to the desired position range
  long targetPosition = map(potValue, 0, 1023, 0, 3600);

  // Move the motor to the target position
  rotation.moveTo(targetPosition);

  // Update the motor position
  rotation.run();
}

//    if (Serial.available() >= BUFFER_SIZE) {
//      Serial.readBytes(buffer, BUFFER_SIZE);
//      // Convert the bytes back to integers
//      int received_integers[4];
//      for (int i = 0; i < 4; i++) {     // remember to change int length here
//        received_integers[i] = 0;
//        for (int j = 0; j < 4; j++) {
//          received_integers[i] |= buffer[i * 4 + j] << (8 * j);
//        }
//      }
//
//      if (received_integers[0] == 0) {    // default: absolute mode
//        //Set Destination and Move
//        rotaPosition = degreesToSteps(received_integers[1]);
//        rotation.moveTo(rotaPosition);
//        isRotateRunning = true;
//
//        tiltPosition = tiltToSteps(received_integers[2]);
////        tiltPosition = received_integers[2];
//        tilt.moveTo(tiltPosition);
//        isTiltRunning = true;
//
//      } else if (received_integers[0] == 1) {   // relative mode
//        int rotaMove = degreesToSteps(received_integers[1]);
//        rotation.move(rotaMove);
//        isRotateRunning = true;
//
//        int tiltMove = tiltToSteps(received_integers[2]);
////        int tiltMove = received_integers[2];
//        tilt.move(tiltMove);
//        isTiltRunning = true;
//      }


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
//int degreesToSteps(int angle){
//  angle = angle % 360;
//  long stepsToMove = (angle / 360.0) * stepsPerRevolution;
//  return stepsToMove;
//}
//
//int tiltToSteps(int tiltVal){
//  //range from 1-20
//  tiltVal = tiltVal % 20;
//  long stepsToMove = (tiltVal / 20.0) * fullTiltSteps;
//  return stepsToMove;
//}
//
//void move_motor_to_home(){
//  //just the rotation for now
//  //move until endstop is HIGH
////  bool rotationHomeState = digitalRead(rotationHomePin);
//  while (!rotationHasHomed) {
////    Serial.println(rotationHasHomed);
//    if (digitalRead(rotationHomePin) != HIGH){
//      //moving motor to home
//      rotation.move(100);
//      rotation.run();
//    } else {
//      //at home location, set this to 0,0,0 postion
//      rotation.setCurrentPosition(0);
//      rotationHasHomed = true;
//    } 
//  }
//}
//
//void move_tilt_to_home(){
//    while (!tiltHasHomed) {
////    Serial.println(rotationHasHomed);
//    if (digitalRead(tiltEndstop) != HIGH){
//      //moving motor to home
//      tilt.move(-100);
//      tilt.run();
//    } else {
//      //at home location, set this to 0,0,0 postion
//      tilt.setCurrentPosition(0);
//      tiltHasHomed = true;
//    } 
//  }
//}
//
//void set_up_motor_stepping_logic(){
//    if (isRotateRunning) {
//    rotation.run();
//  }
//
//  if (isTiltRunning) {
//    tilt.run();
//  }
//
//  if (isFocusRunning) {
//    focus.run();
//  }
//
//  if (rotation.distanceToGo() == 0) {
//    isRotateRunning = false;
//  }
//
//  if (tilt.distanceToGo() == 0) {
//    isTiltRunning = false;
//  }
//
//  if (focus.distanceToGo() == 0) {
//    isFocusRunning = false;
//  }
//}
