#include <SharpDistSensor.h> //distance
#include "DHT.h"  //DHT11
#define relayPin 5                 //relay pin (digital)
#define DHTPIN 7                  //  DHT11 pin (digital)
#define DHTTYPE DHT11            // type of sensor defined as DHT 11
#define pirPin 3              // pin for PIR Sensor HC-SR501 (digital)
#define gasPin 8
DHT dht(DHTPIN, DHTTYPE);       // DHT 11
int gasValue;                  //gas
int calibrationTime = 40;    // PIR sensor
const byte ulstrasnicPin = A0;//distance sensor pin (Analog)
const byte mediumFilterWindowSize = 5;//distance - Window size of the median filter (odd number, 1 = no filtering)
long unsigned int lowIn;   // PIR sensor
//the amount of milliseconds the sensor has to be low
//before we assume all motion has stopped
long unsigned int pause = 2000;      // PIR sensor
boolean lockLow = true;      // PIR sensor
boolean takeLowTime;    // PIR sensor
SharpDistSensor sensor(ulstrasnicPin, mediumFilterWindowSize);//distance -  Create an object instance of the SharpDistSensor class
volatile byte state = LOW;
#include <SparkFun_ADXL345.h>         // SparkFun ADXL345 Library

/*********** COMMUNICATION SELECTION ***********/
/*    Comment Out The One You Are Not Using    */
//ADXL345 adxl = ADXL345(10);           // USE FOR SPI COMMUNICATION, ADXL345(CS_PIN);
ADXL345 adxl = ADXL345();             // USE FOR I2C COMMUNICATION

/****************** INTERRUPT ******************/
/*      Uncomment If Attaching Interrupt       */
int interruptPin = 2;                 // Setup pin 2 to be the interrupt pin (for most Arduino Boards)


/******************** SETUP ********************/
/*          Configure ADXL345 Settings         */
void setup() {
  for (int z = 9; z <= 13; z++)
  {
    pinMode(z, OUTPUT);
    delay(10);
  }
  for (int i=9; i <= 13; i++) {
    digitalWrite(i, HIGH);
    delay(1000);}
  
     for (int i=9; i <= 13; i++) {
    digitalWrite(i, LOW);
    delay(10);}

     for (int i=9; i <= 13; i++) {
    digitalWrite(i, HIGH);
    delay(1000);}
  
     for (int i=9; i <= 13; i++) {
    digitalWrite(i, LOW);
    delay(10);}

  pinMode(pirPin, INPUT);         // PIR sensor
  digitalWrite(pirPin, LOW);    // PIR sensor
  pinMode(gasPin, INPUT);        // microphone
  pinMode(relayPin, OUTPUT);    //relay
//  pinMode(pirPin, INPUT);       //PIR Sensor
 
  dht.begin();                  // DHT 11

  //give some time to calibrate(PIR sensor needs this at least 30 sec)
  Serial.print("calibrating sensor ");
  for (int i = 0; i < calibrationTime; i++) {
    Serial.print(".");
    delay(1000);
  }
  delay(40);
  digitalWrite(13, LOW);         //since setup is completed, turning off

  Serial.begin(9600);                 // Start the serial terminal
  Serial.println("SparkFun ADXL345 Accelerometer Hook Up Guide Example");
  Serial.println();
  adxl.powerOn();                     // Power on the ADXL345
  adxl.setRangeSetting(16);           // Give the range settings
  // Accepted values are 2g, 4g, 8g or 16g
  // Higher Values = Wider Measurement Range
  // Lower Values = Greater Sensitivity
  adxl.setSpiBit(0);                  // Configure the device to be in 4 wire SPI mode when set to '0' or 3 wire SPI mode when set to 1
  // Default: Set to 1
  // SPI pins on the ATMega328: 11, 12 and 13 as reference in SPI Library
  
  adxl.setActivityXYZ(1, 1, 1);       // Set to activate movement detection in the axes "adxl.setActivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
  adxl.setActivityThreshold(75);      // 62.5mg per increment   // Set activity   // Inactivity thresholds (0-255)

  adxl.setInactivityXYZ(1, 0, 0);     // Set to detect inactivity in all the axes "adxl.setInactivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
  adxl.setInactivityThreshold(75);    // 62.5mg per increment   // Set inactivity // Inactivity thresholds (0-255)
  adxl.setTimeInactivity(10);         // How many seconds of no activity is inactive?

  adxl.setTapDetectionOnXYZ(0, 0, 1); // Detect taps in the directions turned ON "adxl.setTapDetectionOnX(X, Y, Z);" (1 == ON, 0 == OFF)

  // Set values for what is considered a TAP and what is a DOUBLE TAP (0-255)
  adxl.setTapThreshold(50);           // 62.5 mg per increment
  adxl.setTapDuration(15);            // 625 μs per increment
  adxl.setDoubleTapLatency(80);       // 1.25 ms per increment
  adxl.setDoubleTapWindow(200);       // 1.25 ms per increment

  // Set values for what is considered FREE FALL (0-255)
  adxl.setFreeFallThreshold(7);       // (5 - 9) recommended - 62.5mg per increment
  adxl.setFreeFallDuration(30);       // (20 - 70) recommended - 5ms per increment

  // Setting all interupts to take place on INT1 pin
  adxl.setImportantInterruptMapping(1, 1, 1, 1, 1);     // Sets "adxl.setEveryInterruptMapping(single tap, double tap, free fall, activity, inactivity);"
  // Accepts only 1 or 2 values for pins INT1 and INT2. This chooses the pin on the ADXL345 to use for Interrupts.
  // This library may have a problem using INT2 pin. Default to INT1 pin.

  // Turn on Interrupts for each mode (1 == ON, 0 == OFF)
  adxl.InactivityINT(1);
  adxl.ActivityINT(1);
  adxl.FreeFallINT(1);
  adxl.doubleTapINT(1);
  adxl.singleTapINT(1);

  pinMode(interruptPin, INPUT);
  //attachInterrupt(digitalPinToInterrupt(interruptPin), ADXL_ISR, RISING);   // Attach Interrupt

  for (int i=9; i <= 13; i++) {
    digitalWrite(i, HIGH);
    delay(10);}
    delay(800);
  
     for (int i=9; i <= 13; i++) {
    digitalWrite(i, LOW);
    delay(10);}
    delay(800);
    
    for (int i=9; i <= 13; i++) {
    digitalWrite(i, HIGH);
    delay(10);}
     delay(800);
  
     for (int i=9; i <= 13; i++) {
    digitalWrite(i, LOW);
    delay(10);}
}
/****************** MAIN CODE ******************/
/*     Accelerometer Readings and Interrupt    */


void(* resetFunc) (void) = 0; //declare reset function @ address 0


void loop() {
  // Accelerometer Readings
  int x, y, z;
  adxl.readAccel(&x, &y, &z);         // Read the accelerometer values and store them in variables declared above x,y,z
 

 
  //to get adxl sensor
if (x>=80 || y>=80 || z>=80) 
{ 
   digitalWrite(13, HIGH);
    delay(2000);

     digitalWrite(13, LOW);
    delay(20);
    
digitalWrite(relayPin, HIGH);
    delay(4000);
    //  Serial.print("activated");
    //  delay(20);
   
    /* Serial.print(x);
  Serial.print(", ");
  Serial.print(y);
  Serial.print(", ");
  Serial.println(z);  */
 }
  // to get gas value
  gasValue = digitalRead(gasPin);
  //Serial.print("Gas: ");
  //Serial.print(gasValue);
  //Serial.print(" \t");
  if (gasValue == LOW)
  {
    digitalWrite(9, HIGH);
    delay(2000);
     digitalWrite(9, LOW);
    delay(10);
    digitalWrite(relayPin, HIGH);
    delay(4000);
    //  Serial.print("activated");
    // Serial.print(" \t");
    //  delay(20);
   
  }
  // to get distance value
  unsigned int distance = sensor.getDist();
  //Serial.print("distance: ");
  //Serial.print(distance);
  //Serial.print(" \t");
  delay(50);
  //if (distance >> 200) {
  // Serial.println("Out of range \t");
  //}
  if (distance <= 200) {
    //  Serial.print("activated");
    // Serial.print(" \t");
    // delay(10);
    digitalWrite(10, HIGH);
    delay(2000);
       digitalWrite(10, LOW);
    delay(10);

    digitalWrite(relayPin, HIGH);
    delay(4000);
  }

  //  to get temp and humidity value from DHT11
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    //  Serial.println("Failed to read from DHT sensor!");
    return;
  }
  /* Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t");
  */
  if (t >= 45)
  {
    digitalWrite(11, HIGH);
    delay(2000);
        digitalWrite(11, LOW);
    delay(10);
    digitalWrite(relayPin, HIGH);
    delay(4000);
  }

  // to get output from PIR Sensor
  if (digitalRead(pirPin) == HIGH) {
    digitalWrite(12, HIGH);
    delay(2000);
digitalWrite(12, LOW);
    delay(10);
    digitalWrite(relayPin, HIGH);
    delay(7500);
    Serial.print("\t motion \t");
    if (lockLow) {
      //makes sure we wait for a transition to LOW before any further output is made:
      lockLow = false;
      Serial.println("---");
      Serial.print("motion detected at ");
      Serial.print(millis() / 1000);
      Serial.println(" sec");
      delay(30);
    }
    takeLowTime = true;
  }
  if (digitalRead(pirPin) == LOW) {
    Serial.print("\t no motion \t");
    digitalWrite(relayPin, LOW);  //the led visualizes the sensors output pin state
    if (takeLowTime) {
      lowIn = millis();          //save the time of the transition from high to LOW
      takeLowTime = false;       //make sure this is only done at the start of a LOW phase
    }
    //if the sensor is low for more than the given pause,
    //we assume that no more motion is going to happen
    if (!lockLow && millis() - lowIn > pause) {
      //makes sure this block of code is only executed again after
      //a new motion sequence has been detected
      lockLow = true;
      Serial.print("motion ended at ");      //output
      Serial.print((millis() - pause) / 1000);
      Serial.println(" sec");
      delay(50);
    }
  } Serial.println("");
  Serial.println("-----------------------------------------------------------------------------------------------------");
  
  ADXL_ISR();
  // You may also choose to avoid using interrupts and simply run the functions within ADXL_ISR();
  //  and place it within the loop instead.
  // This may come in handy when it doesn't matter when the action occurs.


    // to get microphone value
  /* MicValue = digitalRead(micPin);
    Serial.print("Mic1: ");
    Serial.print(MicValue);
    Serial.print(" \t");
    if (MicValue == HIGH)
    { digitalWrite(relayPin, HIGH);
     delay(4000);
    Serial.print("activated");
    Serial.print(" \t");
    delay(20);
    }
    // to get microphone 2 value
    Mic2Value = digitalRead(mic2Pin);
    Serial.print("Mic2: ");
    Serial.print(Mic2Value);
    Serial.print(" \t");
    if (Mic2Value == HIGH)
    { digitalWrite(relayPin, HIGH);
     delay(4000);
     Serial.print("activated");
     Serial.print(" \t");
    delay(20);
    }
  */

}

/********************* ISR *********************/
/* Look for Interrupts and Triggered Action    */
void ADXL_ISR() {

  // getInterruptSource clears all triggered actions after returning value
  // Do not call again until you need to recheck for triggered actions
  byte interrupts = adxl.getInterruptSource();

  // Free Fall Detection
  if (adxl.triggered(interrupts, ADXL345_FREE_FALL)) {
    Serial.println("*** FREE FALL ***");
    //add code here to do when free fall is sensed
    Serial.println("*** TAP ***");
    digitalWrite(13, HIGH);
    delay(2000);
    digitalWrite(13, LOW);
    delay(2000);
    digitalWrite(relayPin, HIGH);
    delay(4000);
  }

  // Inactivity
  if (adxl.triggered(interrupts, ADXL345_INACTIVITY)) {
    Serial.println("*** INACTIVITY ***");
    //add code here to do when inactivity is sensed
  }

  // Activity
  if (adxl.triggered(interrupts, ADXL345_ACTIVITY)) {
    


  }

  // Double Tap Detection
  if (adxl.triggered(interrupts, ADXL345_DOUBLE_TAP)) {
    Serial.println("*** DOUBLE TAP ***");
    //add code here to do when a 2X tap is sensed
  }

  // Tap Detection
  if (adxl.triggered(interrupts, ADXL345_SINGLE_TAP)) {
    Serial.println("*** TAP ***");
    digitalWrite(13, HIGH);
    delay(2000);
    digitalWrite(13, LOW);
    delay(2000);
    digitalWrite(relayPin, HIGH);
    delay(4000);

  }
}
