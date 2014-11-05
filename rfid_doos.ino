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

unsigned int uid;
String uid_txt;

Servo myservo; 
#define servoPin 1

#define pixelPin 3
Adafruit_NeoPixel strip = Adafruit_NeoPixel(4, pixelPin, NEO_GRB + NEO_KHZ800);

unsigned long lastMillis = 0;

boolean fadeDirection = true;
int color = 0;

byte action;

#define ACTION_STANDBY 0
#define ACTION_SCANNING 1
#define ACTION_WAITINGFORSCAN 2
#define ACTION_WRONGSEQUENCE 3
#define ACTION_OPENING 4
#define ACTION_OPEN 5

#define CARD1 66289AB5
#define CARD2 2D5F9AB5
#define CARD3 7EB59AB5


void setup() {
  SPI.begin();
  
  // mifare reader init
  mfrc522.PCD_Init();
  
  // lcd screen init
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("Scan RFID");
  
  // servo init
  myservo.attach(servoPin);
  myservo.write(0);
  
  // neoPixelstrip init
  strip.begin();
  strip.show();
  
  action = ACTION_STANDBY;
}

void loop() {
  switch (action) {
    case ACTION_STANDBY:
      standbymodus();
    break;
  }
  
  
  setColor(255, 0, 0);
  
  if (mfrc522.PICC_IsNewCardPresent()) {
    if (mfrc522.PICC_ReadCardSerial()) {
      lcd.setCursor(0,1);
      lcd.print("Card");
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        lcd.setCursor(i*2, 1);
        if (mfrc522.uid.uidByte[i] < 0x10) {
          lcd.print("0");
        }
        
        lcd.print(mfrc522.uid.uidByte[i], HEX);
        myservo.write(50);
      } 
    }
  }
}

void standbymodus() {
  
  if (fadeDirection == true) {
    color = color + 2;    
    if (color >= 250) {
      fadeDirection = false;
    } 
  } else {
    color = color - 2;    
    if (color <= 30) {
      fadeDirection = true;
    }   
  } 

  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(color, color, color));
  }
  strip.show();    

}


void setColor(int red, int green, int blue) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(red, green, blue));
   }
  strip.show();
}
