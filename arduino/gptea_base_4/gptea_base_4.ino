#include <AccelStepper.h>

//Motor
#define dirPin1 5
#define stepPin1 4
int EN_PIN = 3;
#define motorInterfaceType 1
AccelStepper rotation = AccelStepper(motorInterfaceType, stepPin1, dirPin1);

//encoder
#include <Encoder.h>
#define outputA 10
#define outputB 11
Encoder encoder(outputA, outputB);
long currentPosition = 0;
long previousPosition = 0;
long lastReadings[5] = {0, 0, 0, 0, 0};
int index = 0;

#define BUFFER_SIZE 16 // Set the buffer size based on the size of 4 integers (4 bytes each)
byte buffer[BUFFER_SIZE]; // Create a buffer to store incoming bytes

int currentPotVal = 0;
int recordingState = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


void moveMotorBackAndForth(int repeats) {
 for (int i = 0; i < repeats; i++) {
   rotation.moveTo(100);
   while (rotation.distanceToGo() != 0) {
     rotation.run();
   }
   rotation.moveTo(-100);
   while (rotation.distanceToGo() != 0) {
     rotation.run();
   }
 }
}

void hapticBuzz(){
  digitalWrite(2, LOW);
   delay(100);
   digitalWrite(2, HIGH);
   delay(50);
   digitalWrite(2, LOW);
   delay(100);
   digitalWrite(2, HIGH);
   delay(50);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


void setup() {
 Serial.begin(115200);
 while (!Serial) {
   ;  // wait for serial port to connect. Needed for native USB port only
 }

 //Motor Set Up
 pinMode(EN_PIN, OUTPUT);
 digitalWrite (EN_PIN, LOW);
 rotation.setMaxSpeed(3000);
 rotation.setAcceleration(2000);
  
  pinMode(8, INPUT); //Hall Effect  
  pinMode(2, OUTPUT);  //Haptic Motor
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


void loop() {
 static bool direction = true; // State variable to toggle direction
 int potValue = analogRead(A5);


 // 1. Cup Coaster Dial
 long potVal = map(potValue, 0, 1023, 0, 10);
 if (potVal % 2 == 0){
   if (currentPotVal != potVal){
     Serial.println(potVal);
     hapticBuzz();
     currentPotVal = potVal;
   }
 }

 // 2. Manual Motor control
  int result = checkEncoder();
  if (result == 1) {
    Serial.println("CW");
    rotation.move(50);
  } else if (result == -1) {
    Serial.println("CCW");
    rotation.move(-50);
  }
  rotation.run();
  delay(10);


  // Check if data is available to read from the serial buffer.
  if (Serial.available() > 0) {
    // Read the incoming line as a string.
    String data = Serial.readStringUntil('\n');
    data.trim();
    int receivedNumber = data.toInt();

    // Check if the received number is 5.
    if (receivedNumber == 5) {
      Serial.println("run pour tea sequence ");
      //Blue light
      digitalWrite(6, HIGH);
      delay(3000);
      digitalWrite(6, LOW);
    }
  }

  bool hall = digitalRead(8);  //Hall effect; recording button


  // Check if magnet is detached and we're not already recording
  if (hall == HIGH && recordingState == 0) {  //HIGHT = DETACHED
    Serial.println("Recording audio...");
    Serial.println("1");
    recordingState = 1; // Set recording state
  } 
  // Check if magnet is attached and stop is not already enabled
  else if (hall == LOW && recordingState == 1) {
    Serial.println("Stop recording");
    Serial.println("3");
    Serial.println("submitting audio to API");
    recordingState = 0; // Reset recording state
  }

  // Reset stopState when hall is HIGH and we are already recording
  if (recordingState == 1) {
    //Serial.println("...");
  }

}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

int checkEncoder() {
  currentPosition = encoder.read();

  // Store the current position in the readings buffer
  lastReadings[index] = currentPosition;
  index = (index + 1) % 5; // Move index and wrap around

  // Calculate the average of the last readings
  long averagePosition = 0;
  for (int i = 0; i < 5; i++) {
    averagePosition += lastReadings[i];
  }
  averagePosition /= 5;

  // Check if there's a significant change
  if (averagePosition > previousPosition + 4) { // Threshold for clockwise
    previousPosition = averagePosition;
    return 1;
  } else if (averagePosition < previousPosition - 4) { // Threshold for counterclockwise
    previousPosition = averagePosition;
    return -1;
  }
  return 0; // No significant change
}



 
 // // Only update the target position when needed
 // if (rotation.distanceToGo() == 0) { // Check if motor has reached the target
 //   if (direction) {
 //     rotation.moveTo(100); // Move to 100
 //   } else {
 //     rotation.moveTo(-100); // Move to -100
 //   }
 //   direction = !direction; // Toggle direction
 // }

 // rotation.run();







//   if (Serial.available() >= BUFFER_SIZE) {
//     Serial.readBytes(buffer, BUFFER_SIZE);
//     // Convert the bytes back to integers
//     int received_integers[4];
//     for (int i = 0; i < 4; i++) {     // remember to change int length here
//       received_integers[i] = 0;
//       for (int j = 0; j < 4; j++) {
//         received_integers[i] |= buffer[i * 4 + j] << (8 * j);
//       }
//     }

//     if (received_integers[0] == 0) {    // default: absolute mode
//       //Set Destination and Move
//       rotaPosition = degreesToSteps(received_integers[1]);
//       rotation.moveTo(rotaPosition);
//       isRotateRunning = true;

//       tiltPosition = tiltToSteps(received_integers[2]);
// //        tiltPosition = received_integers[2];
//       tilt.moveTo(tiltPosition);
//       isTiltRunning = true;

//     } else if (received_integers[0] == 1) {   // relative mode
//       int rotaMove = degreesToSteps(received_integers[1]);
//       rotation.move(rotaMove);
//       isRotateRunning = true;

//       int tiltMove = tiltToSteps(received_integers[2]);
// //        int tiltMove = received_integers[2];
//       tilt.move(tiltMove);
//       isTiltRunning = true;
//     }


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
