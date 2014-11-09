#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>

// the pins in use by the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

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
  SPI.begin();
  
  mfrc522.PCD_Init();
  
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("Scan RFID");
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent()) {
    if (mfrc522.PICC_ReadCardSerial()) {
      lcd.setCursor(0,1);
      lcd.print("Card");
      Serial.print("Card UID:");
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        lcd.setCursor(i*2, 1);
        if (mfrc522.uid.uidByte[i] < 0x10) {
          lcd.print("0");
        }
        
        lcd.print(mfrc522.uid.uidByte[i], HEX);

        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
      } 
      Serial.println();      
    }
  }
}
