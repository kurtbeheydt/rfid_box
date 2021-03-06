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

#define MESSAGE_WELCOME   "Dag Timm"

String masterid = "7eb59ab5";//"2d5f9ab5";7E B5 9A B5
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
     
    case ACTION_WRONGSEQUENCE:
      wrongLights();
      delay(1000);
      action = ACTION_STANDBY;
    break;
    
    case ACTION_OPENING:
      openingLights();
      delay(1000);
      action = ACTION_OPEN;
    break;
    
    case ACTION_OPEN:
      myservo.write(50);
    break;
  }
}

void standbymodusLights() {
    strip.setPixelColor(0, strip.Color(0, 0, 200));
    strip.show();
    delay(10);
    strip.setPixelColor(0, strip.Color(0, 0, 0));
    strip.setPixelColor(1, strip.Color(0, 0, 200));
    strip.show();
    delay(10);
    strip.setPixelColor(1, strip.Color(0, 0, 0));
    strip.setPixelColor(2, strip.Color(0, 0, 200));
    strip.show();
    delay(10);
    strip.setPixelColor(2, strip.Color(0, 0, 0));
    strip.setPixelColor(3, strip.Color(0, 0, 200));
    strip.show();
    delay(10);
    strip.setPixelColor(3, strip.Color(0, 0, 0));
    strip.show();
  
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
        if (lastCardIdScanned != masterid) {
          action = ACTION_OPENING;
        } else { 
          lastCardIdScanned = "";
          action = ACTION_WRONGSEQUENCE;
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
    strip.setPixelColor(i, strip.Color(0, 200, 0));
  }
  strip.show();
}

/*
void openingLights() {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  for (int i = 0; i < 2; i++) {
    strip.setPixelColor(0, strip.Color(0, 200, 0));
    strip.show();
    delay(500);
    strip.setPixelColor(0, strip.Color(0, 0, 0));
    strip.setPixelColor(1, strip.Color(0, 200, 0));
    strip.show();
    delay(500);
    strip.setPixelColor(1, strip.Color(0, 0, 0));
    strip.setPixelColor(2, strip.Color(0, 200, 0));
    strip.show();
    delay(500);
    strip.setPixelColor(2, strip.Color(0, 0, 0));
    strip.setPixelColor(3, strip.Color(0, 200, 0));
    strip.show();
    delay(500);
    strip.setPixelColor(3, strip.Color(0, 0, 0));
    strip.show();
  }
}
*/
