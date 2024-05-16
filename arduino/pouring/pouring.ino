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

 pinMode(12, OUTPUT);
 digitalWrite(12, LOW);

 //Motor Set Up
 pinMode(EN_PIN, OUTPUT);
 digitalWrite (EN_PIN, LOW);
 rotation.setMaxSpeed(300);
 rotation.setAcceleration(300);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

void loop() {
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

  // Let's pour it
  pour(pourAmount);
  pourCount++;
  delay(5000); // Delay between pours
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
