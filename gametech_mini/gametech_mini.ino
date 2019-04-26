/*
  Gametech, pilot console controller
  Author: Ryan Williams
  Revised: 4.10.2019
*/

const uint8_t CONSOLE_ID = 0x00;
enum States {IDLING, TRYHARD_A, TRYHARD_B, TRYHARD_C, SUCCESS};
enum Sensors {NONE, B_A, B_B, BOTH};

bool debug_mode = false;

void run_mode();
void read_serial();
void read_sensors();

//// Timing ////
const uint32_t wait_interval = 10000;
unsigned long wait_time = 0x00;
unsigned long C_time = 0xFF;

//// Sensors ////
const int BUTTON1 = A10;
const int BUTTON2 = A9;

//// RESPONSE I/O ////
const int WIN_PIN = A0;


volatile enum States state = IDLING;
volatile enum Sensors sensors = NONE;

void setup() {
  pinMode(BUTTON1, INPUT);
  pinMode(BUTTON2, INPUT);
  pinMode(WIN_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  run_mode();
  delay(10);
}

void run_mode() {
  read_serial(debug_mode);
  switch (state)
  {
    case IDLING:
      wait_time = 0;
      break;
    case TRYHARD_A:       // Press B_A to succeed
      if (debug_mode) Serial.println("In case A");
      //read_sensors();
      if  (digitalRead(BUTTON1)) {
        state = SUCCESS;
      }
      break;
    case TRYHARD_B:       // Press B_B to succeed
      if (debug_mode) Serial.println("In case B");
      if (!wait_time) {
        wait_time = millis();
      }
      if (wait_time - millis() > wait_interval) {
        state = IDLING;
        break;
      }
      read_sensors();
      if  (sensors == B_A) {
        state = SUCCESS;
        break;
      }
      break;
    case TRYHARD_C:       // Hold B_B for >3 sec to succeed
      if (debug_mode) Serial.println("In case C");
      if (!wait_time) {
        wait_time = millis();
        C_time = 0;
      }
      if (wait_time - millis() > wait_interval) {
        state = IDLING;
        break;
      }
      read_sensors();
      if  (sensors == B_B) {
        if (!C_time) {
          C_time = millis();
        }
      } else if (C_time && !sensors == B_B) {
        C_time = 0;
      } else if (millis() - C_time > 3000) {
        state = SUCCESS;
        break;
      }
      break;
    case SUCCESS:
      Serial.write((byte)0xFF);
      if (debug_mode) Serial.println("I won");
      state = IDLING;
      break;
  }
}

void read_sensors() {
  sensors = NONE;
  uint8_t B_A_state = digitalRead(BUTTON1);
  uint8_t B_B_state = digitalRead(BUTTON2);
  Serial.println(B_A_state);
  Serial.println(B_B_state);
  if (B_A_state == HIGH) {
    sensors = B_A;
  } else if (B_B_state == HIGH) {
    sensors = B_B;
  } else if (B_A_state == HIGH && B_B_state == HIGH) {
    sensors = BOTH;
  }
}

// Checks the serial port for a command from the main terminal
void read_serial(bool debug_mode)
{
  uint8_t incoming_byte = 0xAA; //Unused value to prevent false flags
  if (Serial.available() > 0) {
    if (debug_mode) {
      incoming_byte = Serial.parseInt();
    } else {
      incoming_byte = Serial.read();
    }
  }
  switch (incoming_byte)
  {
    case 0x00:        // State request; return 0x00:idling, 0x01:TRYING HARD
      if (state == IDLING) {
        if (debug_mode) Serial.println("I'm not trying");
        Serial.write((byte)0x01);       //I'm idling
      } else {
        if (debug_mode) Serial.println("I'm trying");
        Serial.write((byte)0x00);       //I'm trying hard
      }
      break;
    case 0x01:        //Identify; return console ID
      Serial.write((byte)CONSOLE_ID);
      if (debug_mode) Serial.println("You requested my ID");
      break;
    case 0x09:        //Reset
      state = IDLING;             //TODO: Think about possible consequences of this
      break;
    case 0x10:
      if (debug_mode) Serial.println("going to case A");
      state = TRYHARD_A;
      break;
    case 0x11:
      if (debug_mode) Serial.println("going to case B");
      state = TRYHARD_B;
      break;
    case 0x12:
      if (debug_mode) Serial.println("going to case C");
      state = TRYHARD_C;
      break;
  }
}
