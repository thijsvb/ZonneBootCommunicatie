#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"
#include <Adafruit_GPS.h>

const int FONA_RX = 2;
const int FONA_TX = 3;
const int FONA_RST = 4;
const int GPS_RX = 5;
const int GPS_TX = 6;
const int attopilotI = A5;
const int attopilotU = A4;
const bool GPSECHO = true;  //echoing gps data to Serial monitor

float ISolar;
float USolar;
float Speed;
bool usingInterrupt = false; //Not using interrupt
void useInterrupt(boolean);

// this is a large buffer for replies
char replybuffer[255];
char data[] = "";

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial gpsSS  = SoftwareSerial(GPS_TX,GPS_RX);

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);
Adafruit_GPS GPS(&gpsSS);

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);

int SMStimer = 0;
int SMSdelay = 60000;

void setup(){
  Serial.begin(115200);        //For debugging

  fonaSS.begin(4800);
  GPS.begin(9600);

  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);  //Gps update rate is 1 Hz
  GPS.sendCommand(PGCMD_ANTENNA);
  useInterrupt(true);
  delay(1000);
  gpsSS.println(PMTK_Q_RELEASE);
  
  if(!fona.unlockSIM("0000")){ //Unlocking SIM 
    Serial.println("Unlock failed");
  }
  else{
    Serial.println("Unlocked");
  }
}

SIGNAL(TIMER0_COMPA_vect) {
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
#ifdef UDR0
  if (GPSECHO)
    if (c) UDR0 = c;  
    // writing direct to UDR0 is much much faster than Serial.print 
    // but only one character can be written at a time. 
#endif
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}

void loop(){
  
  if (! usingInterrupt) {
    // read data from the GPS in the 'main loop'
    char c = GPS.read();
    // if you want to debug, this is a good time to do it!
  if (GPSECHO)
      if (c) Serial.print(c);
  }
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences! 
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
  
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }

  if(millis() - SMStimer >= SMSdelay){
    ISolar = analogRead(attopilotI)/3.7;
    USolar = analogRead(attopilotU)/12.99;
    Speed = GPS.speed;

    const String s = String(ISolar) + "," + String(USolar) + "," + String(Speed);
    //Copy s to data
    const int sz = s.length();
    for (int i=0; i!=sz; ++i)
    {
      data[i] = s[i];
    }
    
    if(!fona.sendSMS("0685714499", data)){
      Serial.println("SMS failed");
    }
    else{
      Serial.println("Data sent");
    }
    SMStimer = millis();
  }
}

