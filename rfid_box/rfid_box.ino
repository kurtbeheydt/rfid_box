#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h> // using library from https://github.com/miguelbalboa/rfid/
#include <Servo.h> 
#include <Adafruit_NeoPixel.h>

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

String cardId;

Servo myservo; 
#define servoPin 1

#define pixelPin 3
Adafruit_NeoPixel strip = Adafruit_NeoPixel(4, pixelPin, NEO_GRB + NEO_KHZ800);

unsigned long standbyLastMillis = 0;
boolean fadeDirection = true;
int color = 0;

byte action;

#define ACTION_STANDBY 0
#define ACTION_SCANNING 1
#define ACTION_WAITINGFORNEXTSCAN 2
#define ACTION_WRONGSEQUENCE 3
#define ACTION_OPENING 4
#define ACTION_OPEN 5

#define MESSAGE_WELCOME   "Welkom reiziger"
#define MESSAGE_WELCOME2  "Leg een kaart  "
#define MESSAGE_CORRECT   "Juiste kaart   "
#define MESSAGE_SCANNEXT  "Volgende kaart "
#define MESSAGE_OPENING   "Bijna open...  "
#define MESSAGE_WRONG     "Verkeerde kaart"
#define MESSAGE_TRYAGAIN  "Probeer opnieuw"
#define MESSAGE_TIMEOUT   "Te laat!       "
#define MESSAGE_OPEN      "Veel plezier!  "
#define MESSAGE_EMPTY     "               "

String cardids[] = {"a31796f4", "66289ab5", "73d391f4", "7eb59ab5", "2d5f9ab5"};
String masterid = "6098bf6e";
int CARD_COUNT = 5;
int lastCardScanned = 0;
String lastCardIdScanned;
unsigned long CardScanLastMillis = 0;

void setup() {
  SPI.begin();
  
  // mifare reader init
  mfrc522.PCD_Init();
  
  // lcd screen init
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print(MESSAGE_WELCOME);
  lcd.setCursor(0,1);
  lcd.print(MESSAGE_WELCOME2);
  
  // servo init
  myservo.attach(servoPin);
  myservo.write(1);
  
  // neoPixelstrip init
  strip.begin();
  strip.show();
  
  action = ACTION_STANDBY;
}


void loop() {
  switch (action) {
    case ACTION_STANDBY:
      standbymodusLights();
      readCard();
    break;
    
    case ACTION_SCANNING:
      standbymodusLights();
    break;
     
    case ACTION_WAITINGFORNEXTSCAN:
      readCard();
      statusLights();
      if (millis() - CardScanLastMillis > 5000) {
        lcd.setCursor(0, 0);
        lcd.print(MESSAGE_TIMEOUT);
        lcd.setCursor(0, 1);
        lcd.print(MESSAGE_TRYAGAIN);
        action = ACTION_STANDBY;
        lastCardScanned = 0;
        lastCardIdScanned = "";
      }
    break;
    
    case ACTION_WRONGSEQUENCE:
      wrongLights();
      delay(1000);
      action = ACTION_STANDBY;
    break;
    
    case ACTION_OPENING:
      openingLights();
      action = ACTION_OPEN;
    break;
    
    case ACTION_OPEN:
      lcd.setCursor(0, 0);
      lcd.print(MESSAGE_OPEN);
      lcd.setCursor(0, 1);
      lcd.print(MESSAGE_EMPTY);
      myservo.write(50);
    break;
  }
}

void standbymodusLights() {
  if (millis() - standbyLastMillis > 10) {
    standbyLastMillis = millis();
    
    if (fadeDirection == true) {
      color = color + 1;    
      if (color >= 250) {
        fadeDirection = false;
      } 
    } else {
      color = color - 1;    
      if (color <= 30) {
        fadeDirection = true;
      }   
    } 
  
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(color, color, color));
    }
    
    strip.show();
  }
}

void readCard() {
  if (mfrc522.PICC_IsNewCardPresent()) {
    if (mfrc522.PICC_ReadCardSerial()) {
      cardId = "";
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        cardId += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
        cardId += String(mfrc522.uid.uidByte[i], HEX);
      } 
            
      if (lastCardIdScanned != cardId) {
        lastCardIdScanned = cardId;
        if (lastCardIdScanned == masterid) {
          action = ACTION_OPEN;
        } else if (lastCardIdScanned == cardids[lastCardScanned]) {
          lastCardScanned++;
          if (lastCardScanned >= CARD_COUNT) { // all cards are scanned
            action = ACTION_OPENING;
            lcd.setCursor(0, 0);
            lcd.print(MESSAGE_CORRECT);
            lcd.setCursor(0, 1);
            lcd.print(MESSAGE_OPENING);            
          } else { // correct card in this sequence
            action = ACTION_WAITINGFORNEXTSCAN;
            lcd.setCursor(0, 0);
            lcd.print(MESSAGE_CORRECT);
            lcd.setCursor(0, 1);
            lcd.print(MESSAGE_SCANNEXT);
            CardScanLastMillis = millis();
          }
        } else { // wrong card in this sequence
          lastCardScanned = 0;
          lastCardIdScanned = "";
          action = ACTION_WRONGSEQUENCE;
          lcd.setCursor(0, 0);
          lcd.print(MESSAGE_WRONG);
          lcd.setCursor(0, 1);
          lcd.print(MESSAGE_TRYAGAIN);
        }        
      }
    }
  } 
}

void statusLights() {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();
  for (int i = 0; i < lastCardScanned; i++) {
    strip.setPixelColor(i, strip.Color(0, 200, 0));
  }
  strip.show();
}

void wrongLights() {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(200, 0, 0));
  }
  strip.show();
}

void openingLights() {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  for (int i = 0; i < 2; i++) {
    strip.setPixelColor(0, strip.Color(0, 0, 200));
    strip.show();
    delay(500);
    strip.setPixelColor(0, strip.Color(0, 0, 0));
    strip.setPixelColor(1, strip.Color(0, 0, 200));
    strip.show();
    delay(500);
    strip.setPixelColor(1, strip.Color(0, 0, 0));
    strip.setPixelColor(2, strip.Color(0, 0, 200));
    strip.show();
    delay(500);
    strip.setPixelColor(2, strip.Color(0, 0, 0));
    strip.setPixelColor(3, strip.Color(0, 0, 200));
    strip.show();
    delay(500);
    strip.setPixelColor(3, strip.Color(0, 0, 0));
    strip.show();
  }
}
