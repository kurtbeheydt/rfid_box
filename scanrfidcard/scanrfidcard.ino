#include <SPI.h>
#include <MFRC522.h>

/*
 * SPI MOSI   11
 * SPI MISO   12
 * SPI SCK    13
 */

#define SS_PIN 10 // SDA
#define RST_PIN 2

MFRC522 mfrc522(SS_PIN, RST_PIN);

unsigned int uid;
String uid_txt;

void setup() {
  Serial.begin(9600);
  Serial.println("ok");
  SPI.begin();
  
  mfrc522.PCD_Init();
  
  pinMode(13, OUTPUT);
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent()) {
    if (mfrc522.PICC_ReadCardSerial()) {
      digitalWrite(13, HIGH);
      delay(500);
      digitalWrite(13, LOW);
    }
  }
}
