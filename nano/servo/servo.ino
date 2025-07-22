const uint8_t pinRC1 = 9;
const uint8_t pinRC2 = 10;
const uint16_t MIN = 2000;// for 180 deg servo: 500 ms
const uint16_t MAX = 4000;// for 180 deg servo: 2500 ms
const uint16_t RESOLUTION = (MAX - MIN) / 1000;
const uint32_t PERIOD = 39999;

void setup() {
  TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM11);
  TCCR1B = _BV(WGM13) |_BV(WGM12) |_BV(CS11);
  ICR1 = PERIOD;
  OCR1B = OCR1A = MIN + (MAX - MIN) /2; // initialize to the middle;
  pinMode(pinRC1, OUTPUT);
  pinMode(pinRC2, OUTPUT);
  Serial.begin(115200);
}
void loop() {
  uint32_t receivedData = 0;
  char response;
  if (Serial.available() >= 4) {
    Serial.readBytes((char*)&receivedData, sizeof(receivedData));
    uint16_t v = receivedData & 0xFFFF;
    if ((receivedData >> 28) == 0xF) {
      // this is test for connection
      response = 1;
      Serial.write(&response);
    } else {
      // this is a rotate_to command
      v = MIN + RESOLUTION * v; // scale v
      if (v > MAX) {
        v = MAX;
      }
      if (v < MIN) {
        v = MIN;
      }
      
      uint8_t chan = (receivedData >> 28) & 0x0F;
      if (chan == 0) {
        OCR1A = v;
      } else if (chan == 1) {
        OCR1B = v;
      }
      response = 2;
      Serial.write(&response);
    }
  }
  delay(10);
}
