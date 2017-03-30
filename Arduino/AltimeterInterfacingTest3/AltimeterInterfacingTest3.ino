#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

SoftwareSerial mySerial(8, 7); // RX, TX 10 11
File logFile;

int buttonPin = 2;
char inputByte;
byte byteArray[10];
int index = 0;
int val = 0;
int maxAlt = 0;



void setup() {
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);
  while( !Serial ){} //wait until serial port is opened
  Serial.println("Serial initialized");

  mySerial.begin(9600);
  while( !mySerial ) {} //wait until software serial is opened
  Serial.println("Software Serial initialized");
  
  Serial.println("Begining SD card initialization.");
  if( !SD.begin(4)) { //if the SD card with Chip Select (CS) pin 4 not connected
    Serial.println("Failed to initialize SD card");
    return;
  }
  Serial.println("SD card initialized succesfully");

  logFile = SD.open("log.txt", FILE_WRITE);

  
  

}

void loop() {
  if( !logFile ) {
    Serial.println("Error: Could not open file");
    return;
  }
  if (mySerial.available()) {
    byte value = mySerial.read();
    Serial.write(value); //read byte from mySerial and output ascii value
    logFile.write(value);
    }
  //////////FIX ME: Change to check and see if last 5 alt values are 0 or less than 0 to see if flight is over
  if( digitalRead(buttonPin) == HIGH) { //if button is pressed
    logFile.close(); //close file for writing
    Serial.println("Log stopped");
    delay(500);
    logFile = SD.open("Log.txt", FILE_READ); //opening file for reading/data processing
    Serial.println("Log file opened for processing");
    delay(500);
    while(logFile.available()) { //reading in byte by byte from sd card
      inputByte = logFile.read();
      index = 0;
      while(inputByte != 10) {//while not new line 
        byteArray[index] = inputByte;
        inputByte = logFile.read();
        index++; 
      }
      if( index == 2 ) {
        val = OneDigit();
        Serial.println(val);
      }
      if( index == 3 ) {
        val = TwoDigit();
        Serial.println(val);
      }
      if( index == 4 ) {
        val = ThreeDigit();
        Serial.println(val);
      }
      if( val > maxAlt ) {
        maxAlt = val;
      }
      ClearArray();
    }
    logFile.close(); //closing file for reading
    Serial.print("Max Altitude (AGL): ");
    Serial.println(maxAlt);
    while(1) {
      //do nothing, stopping program
    }
    return;
  }
}



///////////////////////Functions//////////////////////////////

void ClearArray() {
  for(int i = 0; i < 10; i++) {
    byteArray[i] = 0;
  }
  return;
}
int OneDigit() {
    int ones = (int)byteArray[0] - 48;
    int total = ones;
    return total;
}
int TwoDigit() {
    int ones = (int)byteArray[1] - 48;
    int tens = ((int)byteArray[0] - 48) * 10;
    int total = ones + tens;
    return total;
}
int ThreeDigit() {
    int ones = (int)byteArray[2] - 48;
    int tens = ((int)byteArray[1] - 48) * 10;
    int hundreds = ((int)byteArray[0] - 48) * 100;
    int total = ones + tens + hundreds;
    return total;
}

