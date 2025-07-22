// Servo settings
const uint8_t pinRC1 = 9;
const uint8_t pinRC2 = 10;
const uint16_t SEROV_PWM_MIN = 2000;// for 180 deg servo: 500 ms
const uint16_t SEROV_PWM_MAX = 4000;// for 180 deg servo: 2500 ms
const uint16_t RESOLUTION = 1000;
const uint32_t PERIOD = 39999;

// Motor settings, GPIO and PWM Pins
const uint8_t gpioPins[4] = {2, 3, 4, 5};  // 4 GPIO pins
const uint8_t pwmPins[2] = {6, 11};        // 2 additional PWM pins
const uint8_t MOTOR_PWM_MIN = 0;
const uint8_t MOTOR_PWM_MAX = 255;

// message types
#define TYPE_BYTE 24
#define INIT_TYPE 0xF
#define SERVO_TYPE 0xE
#define MOTOR_PWM_TYPE 0xD
#define MOTOR_DIR_TYPE 0xC

// device index
#define DEV_BYTE 16
#define DEV_1 1
#define DEV_2 2
#define INIT_MSG 0xF
#define DATA_MASK 0xFFFF

#define MSG_MASK 0xFFFFFFFF

#define ERROR 0
#define SUCCESS_INIT 1
#define SUCCESS_ROTATE 2
#define SUCCESS_MOTOR_SPEED 3
#define SUCCESS_MOTOR_DIRECTION 4



void setup() {
  TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM11);
  TCCR1B = _BV(WGM13) |_BV(WGM12) |_BV(CS11);
  ICR1 = PERIOD;
  OCR1B = OCR1A = SEROV_PWM_MIN + (SEROV_PWM_MAX - SEROV_PWM_MIN) / 2; // initialize to the middle;

  pinMode(pinRC1, OUTPUT);
  pinMode(pinRC2, OUTPUT);

  // Set GPIO Pins as OUTPUT
  for (uint8_t i = 0; i < 4; i++) {
    pinMode(gpioPins[i], OUTPUT);
    digitalWrite(gpioPins[i], LOW); // Initialize to LOW
  }

  // Set PWM pins as OUTPUT
  for (uint8_t i = 0; i < 2; i++) {
    pinMode(pwmPins[i], OUTPUT);
  }

  Serial.begin(115200);
}

void loop() {
  if (Serial.available() < 4) {
    delay(10); // delay 10 ms until available >= 4, don't write any message
  }


  uint32_t receivedData = 0;
  Serial.readBytes((char*)&receivedData, sizeof(receivedData));
  char type     = (receivedData >> TYPE_BYTE) & 0xFF;
  char device   = (receivedData >> DEV_BYTE ) & 0xFF;
  uint16_t data = (receivedData >> 0        ) & DATA_MASK;
  char response = ERROR;
  switch (type) {
    
    // this is test for connection
    case INIT_TYPE: {
      response = SUCCESS_INIT;
      break;
    }

    // this is a servo rotate_to command
    case SERVO_TYPE: {
      data = SEROV_PWM_MIN + ((SEROV_PWM_MAX - SEROV_PWM_MIN) / RESOLUTION) * data; // scale data with MIN and MAX
      if (data > SEROV_PWM_MAX) {
        data = SEROV_PWM_MAX;
      }
      if (data < SEROV_PWM_MIN) {
        data = SEROV_PWM_MIN;
      }

      switch (device) {
        case DEV_1: {
          OCR1A = data;
          response = SUCCESS_ROTATE;
          break;
        }
        
        case DEV_2: {
          OCR1B = data;
          response = SUCCESS_ROTATE;
          break;
        }

      }
      break;
    }

    // this is a motor speed command
    case MOTOR_PWM_TYPE: {
      
      if (data > MOTOR_PWM_MAX) {
        data = MOTOR_PWM_MAX;
      }
      if (data < MOTOR_PWM_MIN) {
        data = MOTOR_PWM_MIN;
      }

      switch (device) {
        case DEV_1: {
          analogWrite(pwmPins[0], data & 0xFF);
          response = SUCCESS_MOTOR_SPEED;
          break;
        }
        
        case DEV_2: {
          analogWrite(pwmPins[1], data & 0xFF);
          response = SUCCESS_MOTOR_SPEED;
          break;
        }

      }
      break;
    }

    // this is a motor drection command
    case MOTOR_DIR_TYPE: {
      bool direction = data & 0XFF;
      switch (device) {
        case DEV_1: {
          digitalWrite(gpioPins[0], direction ? HIGH : LOW);
          digitalWrite(gpioPins[1], direction ? LOW : HIGH);
          response = SUCCESS_MOTOR_DIRECTION;
          break;
        }
        
        case DEV_2: {
          digitalWrite(gpioPins[2], direction ? HIGH : LOW);
          digitalWrite(gpioPins[3], direction ? LOW : HIGH);
          response = SUCCESS_MOTOR_DIRECTION;
          break;
        }

      }
      break;
    }

  }
  Serial.write(&response);
  delay(10);
}
