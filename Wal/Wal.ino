#include <SoftwareSerial.h>
#include <Adafruit_FONA.h>

#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4

char replybuffer[255];

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

void setup() {
  while(!Serial);
  
  Serial.begin(115200);
  Serial.println("Wacht...");
  
  
  fonaSS.begin(4800);
  
  if(!fona.begin(fonaSS)){
    Serial.println("Error0");
    while(1);
  }
  Serial.println("Unlock...");
  
  if(!fona.unlockSIM("0000")){
    Serial.println("Error1");
  }
  else{
    Serial.println("Unlocked");
  }
}

void loop() {
  const uint8_t smsn = fona.getNumSMS();
  if(!fona.getSMSSender(smsn, replybuffer, 250)){
    Serial.println("Error2");
  }
  const uint16_t smslen;
  if(!fona.readSMS(smsn, replybuffer, 250, &smslen)){
    Serial.println("Error3");
  }
  Serial.println(replybuffer);
}
