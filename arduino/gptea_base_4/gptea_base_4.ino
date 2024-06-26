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
 rotation.setMaxSpeed(300);
 rotation.setAcceleration(300);
  
  pinMode(8, INPUT); //Hall Effect  
  pinMode(2, OUTPUT);  //Haptic Motor

  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


void loop() {
  int potValue = analogRead(A5);

  //0. pouring SETUP
  static bool ledShouldBeOn = false;  // Flag to control the LED state
  int pourAmount = 0;                 // Variable to control the amount of each pour
  static int pourCount = 0;           // Total number of pours to perform
   // Update pourAmount based on pourCount
  if (pourCount == 0) {
    ledShouldBeOn = true;  // Activate LED when no pours have been performed
  } else if (pourCount == 1) {
    pourAmount = -300;
    ledShouldBeOn = false;
  } else if (pourCount == 2) {
    pourAmount = -400;
    ledShouldBeOn = false;
  } else if (pourCount == 3) {
    pourAmount = -500;
    ledShouldBeOn = false;
  } else if (pourCount > 3) {
    Serial.println("All cycles complete. Entering long sleep...");
    delay(10000); // Long delay to observe if resetting occurs
    pourCount = 0;  // Reset pour count for next round
    ledShouldBeOn = true;  // Reset the LED control flag
    return;  // Exit the loop to start over
  }
  // Update LED state
  digitalWrite(12, ledShouldBeOn ? HIGH : LOW);



  
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


  // 3. Pouring: Check if data is available to read from the serial buffer.
  if (Serial.available() > 0) {
    // Read the incoming line as a string.
    String data = Serial.readStringUntil('\n');
    data.trim();
    int receivedNumber = data.toInt();

    // Check if the received number is 5.
    if (receivedNumber == 5) {
      Serial.println("run pour tea sequence ");
      pour(pourAmount);
      pourCount++;
      delay(1000);
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




void pour(int pourAmount) {

  digitalWrite(6, HIGH);
  // First move to -500
  rotation.setMaxSpeed(300);
  rotation.setAcceleration(300);
  Serial.println("Moving to -500 (CCW)");
  rotation.moveTo(pourAmount);  // Set the target to -500
  while (rotation.distanceToGo() != 0) {
    rotation.run(); // Block here until it reaches -500
  }
  delay(1000);
  rotation.setMaxSpeed(2000);
  rotation.setAcceleration(2000);
  // After reaching -500, immediately move to 0
  Serial.println("Moving to 0 (CW)");
  rotation.moveTo(0);  // Set the target to 0
  while (rotation.distanceToGo() != 0) {
    rotation.run(); // Block here until it reaches 0
  }
  digitalWrite(6, LOW);
  Serial.println("Cycle Complete");
}
