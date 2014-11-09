rfid treasure box
=================

Box with rfid reader, lcd display and servo to open the box.
Built as a treasure hunt for a birthday party of 7-year old daughter.

Children had to collect five RFID-cards and swipe them in a correct order over the box to open the box and find the chocolate coins inside.

Arduino code: https://raw.githubusercontent.com/kurtbeheydt/rfid_box/master/rfid_box/rfid_box.ino

Helper code to get the ID's of your cards: https://raw.githubusercontent.com/kurtbeheydt/rfid_box/master/scanrfidcard/scanrfidcard.ino


non-default arduino libraries used:
- MFRC522 - https://github.com/miguelbalboa/rfid/
- Adafruit_NeoPixel - https://github.com/adafruit/Adafruit_NeoPixel