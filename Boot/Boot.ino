#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"

#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4

// this is a large buffer for replies
char replybuffer[255];
char data[] = "";

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);

int SMStimer = 0;
int SMSdelay = 60000;

void setup(){
  Serial.begin(115200);        //For debugging

  fonaSS.begin(4800);
  
  if(!fona.unlockSIM("0000")){ //Unlocking SIM 
    Serial.println("Unlock failed");
  }
  else{
    Serial.println("Unlocked");
  }
}

void loop(){

  if(millis() - SMStimer >= SMSdelay){
    if(!fona.sendSMS("0685714499", data)){
      Serial.println("SMS failed");
    }
    else{
      Serial.println("Data sent");
    }
    SMStimer = millis();
  }
}

