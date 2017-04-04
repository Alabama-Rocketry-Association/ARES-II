#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <SD.h>

File logfile;

SoftwareSerial mySerial(8, 7);
Adafruit_GPS GPS(&mySerial);

#define GPSECHO  false

void setup()  
{
  Serial.begin(115200);
  // delay(5000);

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  
  // RMC (recommended minimum) and GGA (fix data) including altitude
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // "minimum recommended" data
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  GPS.sendCommand(PMTK_API_SET_FIX_CTL_1HZ); // 1 Hz position fix update rate

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  // delay(1000);
  // Ask for firmware version
  mySerial.println(PMTK_Q_RELEASE);

  //Setting up logfile with incrementing name
  char filename[15];
  strcpy(filename, "GPSLOG00.TXT");
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = '0' + i/10;
    filename[7] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }

  logfile = SD.open(filename, FILE_WRITE);
  if( ! logfile ) {
    Serial.print("Couldnt create "); 
    Serial.println(filename);
    return;
  }
  Serial.print("Writing to "); 
  Serial.println(filename);
}

uint32_t timer = millis();

void loop()                     // run over and over again
{
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if ((c) && (GPSECHO))
    Serial.write(c); 
  
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
  
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }

  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 1000) { 
    timer = millis(); // reset the timer
    
    if (GPS.fix) {
      float intPartLat = floor(GPS.latitude / 100.0);
      float intPartLon = floor(GPS.longitude / 100.0);
      float decPartLat = ( (GPS.latitude / 100.0) - intPartLat ) / 0.6;
      float decPartLon = ( (GPS.longitude / 100.0) - intPartLon ) / 0.6;
      GPS.latitude =  intPartLat + decPartLat;
      GPS.longitude = intPartLon + decPartLon;
      if (GPS.lat == 'S') GPS.latitude *= -1.0;
      if (GPS.lon == 'W') GPS.longitude *= -1.0;
      Serial.print(GPS.latitude, 4); // Serial.print(GPS.lat);
      Serial.print(", "); 
      Serial.println(GPS.longitude, 4); // Serial.print(GPS.lon);
      logfile.print(GPS.latitude, 4);
      logfile.print(", ");
      logfile.println(GPS.longitude, 4);
      /*Serial.print(", ");
      Serial.print(GPS.hour, DEC); Serial.print(':');
      Serial.print(GPS.minute, DEC); Serial.print(':');
      Serial.print(GPS.seconds, DEC); Serial.print('.');
      Serial.println(GPS.milliseconds);*/
      
      // Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      // Serial.print("Altitude: "); Serial.println(GPS.altitude);
    }
  }
}
