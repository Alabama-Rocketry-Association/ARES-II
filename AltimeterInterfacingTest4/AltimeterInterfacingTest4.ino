#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

SoftwareSerial mySerial(8, 7); // RX, TX 10 11
File logFile;

bool DEBUG_MODE = false; //change to true to see output while parsing log file after landing

const int LAUNCH_DETECT = 200; //launch detection height in ft (AGL)
const int LANDING_DETECT = 10; //landing detection height in ft (AGL). Greater than zero to account for varriations in terrain.

int buttonPin = 2;
char inputByte;
byte byteArray[10];
int index = 0;
int val = 0;
int maxAlt = 0;
int lastAlt[5];
int j = 0;



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
    byteArray[index] = value;
    index++;
    if(value == 10) {//convert and check if max alt
      val = GetAltitude(index);
      lastAlt[j] = val;
      j++;
      if( j > 4 ) { // if lastAlt array is full, start overwriting data from begining of array
        j = 0; 
      }
    }      
  }
  
  if( maxAlt > LAUNCH_DETECT && lastAlt[0] < LANDING_DETECT && lastAlt[1] < LANDING_DETECT && lastAlt[2] < LANDING_DETECT && lastAlt[3] < LANDING_DETECT && lastAlt[4] < LANDING_DETECT) { //if rocket has landed

    Serial.println("Rocket has landed.");
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
      val = GetAltitude(index);
      if( DEBUG_MODE == true ) { Serial.println(val); }
      if( val > maxAlt ) {
        maxAlt = val;
        if( DEBUG_MODE == true ) { Serial.print("Maximum altitude updated to: "); Serial.println(maxAlt); }
      }
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
int FourDigit() {
    int ones = (int)byteArray[3] - 48;
    int tens = ((int)byteArray[2] - 48) * 10;
    int hundreds = ((int)byteArray[1] - 48) * 100;
    int thousands = ((int)byteArray[0] - 48) * 1000;
    int total = ones + tens + hundreds + thousands;
    return total;
}
int FiveDigit() {
    int ones = (int)byteArray[4] - 48;
    int tens = ((int)byteArray[3] - 48) * 10;
    int hundreds = ((int)byteArray[2] - 48) * 100;
    int thousands = ((int)byteArray[1] - 48) * 1000;
    int tenThousands = ((int)byteArray[0] - 48) * 10000;
    int total = ones + tens + hundreds + thousands + tenThousands;
    return total;
}
int SixDigit() {
    int ones = (int)byteArray[5] - 48;
    int tens = ((int)byteArray[4] - 48) * 10;
    int hundreds = ((int)byteArray[3] - 48) * 100;
    int thousands = ((int)byteArray[2] - 48) * 1000;
    int tenThousands = ((int)byteArray[1] - 48) * 10000;
    int hundredThousands = ((int)byteArray[0] - 48) * 100000;
    int total = ones + tens + hundreds + thousands + tenThousands + hundredThousands;
    return total;
}
int GetAltitude(int len) {
  int temp;
  if (len == 2) { temp = OneDigit(); }
  else if (len == 3) { temp = TwoDigit(); }
  else if (len == 4) { temp = ThreeDigit(); }
  else if (len == 5) { temp = FourDigit(); }
  else if (len == 6) { temp = FiveDigit(); }
  else if (len == 7) { temp = SixDigit(); }
  else { temp = -100000; } // error case
  ClearArray();
  return temp;
}

