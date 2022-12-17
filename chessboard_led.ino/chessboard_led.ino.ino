#include <ArduinoBLE.h>
// #include <LiquidCrystal.h>
#include "path.c"

/*
    File to specify Arduino BLE and interaction with the reed sensor mux chip.

    Hardware Hookup:
        Mux Breakout ----------- Arduino
        COM -------------------- 15
        S0 --------------------- 19/A0
        S1 --------------------- 20/A1
        S2 --------------------- 21/A2
        S3 --------------------- 22/A3
        EN --------------------- 13, 7, 8, 9
        VCC -------------------- 3V3
        GND -------------------- GND

        Magnet ----------------- 6

        Motor Control 
        Dir 1 ------------------ 5
        Step 1 ----------------- 4
        Dir 2 ------------------ 3
        Step 2 ----------------- 2       
*/

// LED_BUILTIN = Bluetooth Connection Status
// LEDR = Error
// LEDG = Magnet
// LEDB = Scan Board

// Pin Definitions
const int SELECT_PINS[4] = {A0, A1, A2, A3}; // A0, A1, A2, A3
const int ENABLE_MUX_PINS[4] = {13, 7, 8, 9}; // D13, D7, D8, D9
const int COM_PIN = 15;
const int MAGNET_PIN = 6;
const int SWITCH1_PIN = 10;
const int SWITCH2_PIN = A4;
const int LED_ON_TIME = 500;  // Each LED is on for 0.5s


const int DIR1_PIN = 5;
const int DIR2_PIN = 3;
const int STEP1_PIN = 4;
const int STEP2_PIN = 2;

const int STEPS_PER_SQUARE = 200;

BLEService service("4400b98a-0b70-4253-b250-d60e21f0f224");
BLECharacteristic command("d7a16eff-1ee7-4344-a3d2-a8203d97d75c", BLERead | BLEWrite, 5);

int loc = 240;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  setupBLE();
  /* setupMux(); */
  setupMagnet();
  setupMotor();
  setupSwitches();

	for (int i=0; i<8; i++)
		board[i+0] = board[i+8] = board[i+48] = board[i+56] = 1;

  switchCalibration();

	digitalWrite(LEDR, LOW);
	digitalWrite(LEDG, LOW);
	digitalWrite(LEDB, LOW);
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("Setup complete.");
}

void setupBLE(){
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");
    digitalWrite(LEDR, HIGH);
    while (1);
  }

  BLE.setLocalName("AutomatedChess");
  BLE.setAdvertisedService(service);

  service.addCharacteristic(command);
  BLE.addService(service);

  BLE.advertise();
}

void setupMux() {
  for (int i = 0; i < 4; i++) {
    pinMode(SELECT_PINS[i], OUTPUT);
    digitalWrite(SELECT_PINS[i], LOW);
    pinMode(ENABLE_MUX_PINS[i], OUTPUT);
    digitalWrite(ENABLE_MUX_PINS[i], LOW); //FIXME: check pull up/down on EN
  }
  pinMode(COM_PIN, INPUT);
}

void setupSwitches() {
  pinMode(SWITCH1_PIN, INPUT_PULLUP);
  pinMode(SWITCH2_PIN, INPUT_PULLUP);
}

void setupMagnet() {
  pinMode(MAGNET_PIN, OUTPUT);
  digitalWrite(MAGNET_PIN, LOW);
}

void setupMotor() {
  pinMode(DIR1_PIN, OUTPUT);
  pinMode(DIR2_PIN, OUTPUT);
  pinMode(STEP1_PIN, OUTPUT);
  pinMode(STEP2_PIN, OUTPUT);
}

inline void stepMotor() {
	digitalWrite(STEP1_PIN, HIGH);
	digitalWrite(STEP2_PIN, HIGH);
	delayMicroseconds(1000);
	digitalWrite(STEP1_PIN, LOW);
	digitalWrite(STEP2_PIN, LOW);
	delayMicroseconds(1000);
}

void switchCalibration(){
  Serial.println("Switch Calibration");

/* 	while (true) { */
/* 		Serial.print(digitalRead(SWITCH1_PIN)); */
/* 		Serial.print(digitalRead(SWITCH2_PIN)); */
/* 		Serial.println(); */
/* 		delay(500); */
/* 	} */

  digitalWrite(DIR1_PIN, HIGH);
  digitalWrite(DIR2_PIN, LOW);
  while (digitalRead(SWITCH2_PIN) == HIGH) stepMotor();
  Serial.println("HIT LEFT");

  digitalWrite(DIR1_PIN, LOW);
  digitalWrite(DIR2_PIN, LOW);
  while (digitalRead(SWITCH1_PIN) == HIGH) stepMotor();
  Serial.println("HIT TOP");

	// Create some slack on the left side by moving the trolley to the right by a bit.
  digitalWrite(DIR1_PIN, LOW);
  digitalWrite(DIR2_PIN, HIGH);
  for (int i=0; i<50; i++) stepMotor();
  digitalWrite(DIR1_PIN, HIGH);
  digitalWrite(DIR2_PIN, HIGH);
  for (int i=0; i<30; i++) stepMotor();
}


void loop() {
  // Wait until device is connected, check every second and blink built-in led.
  BLEDevice central = BLE.central();

	if (Serial.available()) {
		executeCommand(Serial.readString());
		printBoard();
	}

  if (central) {
    Serial.print("Connected to central:");
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, HIGH);  // turn on the LED to indicate the connection
    //scanBoard();
    printBoard();
    while (central.connected()) {
      if (command.written()) {
        executeCommand((const char*) command.value());
        //scanBoard();
        printBoard();
      }
    }
    digitalWrite(LED_BUILTIN, LOW);  // turn on the LED to indicate the connection
    Serial.print("Disconnected from central");
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
  }

  delay(500);
}

void executeCommand(String cmd) {
  // Parses a command like A3-B2 and resolves it as a src and dst integer
  // between 0 and 63 which corresponds to the cell.
  int src = to16(cmd[0]-'A' + 8*(cmd[1]-'1'));
  int dst = to16(cmd[3]-'A' + 8*(cmd[4]-'1'));
	
	if (src < 0 || src >= 256 || dst < 0 || dst >= 245 ) {
		Serial.println("Invalid command");
		return;
	}

	Serial.print("Running Move: ");
  Serial.println(cmd);

  enum direction path[64];
  bool capture = board[dst];

  if (capture) {
		Serial.println("Move to dst");
    findPath(loc, dst, path);
    movePath(path);

		Serial.println("Move piece to capture jail");
    digitalWrite(MAGNET_PIN, HIGH);
    findPath(dst, 15, path);
    movePath(path);
    digitalWrite(MAGNET_PIN, LOW);

		Serial.println("Move to start");
    findPath(15, src, path);
    movePath(path);
  } else {
		Serial.println("Move to start");
    findPath(loc, src, path);
    movePath(path);
  }

	Serial.println("Move piece to final location");
  digitalWrite(MAGNET_PIN, HIGH);
	board[to8(src)] = 0;
	board[to8(dst)] = 1;
  findPath(src, dst, path);
  movePath(path);
  digitalWrite(MAGNET_PIN, LOW);

  loc = dst;
}

void movePath(enum direction *path) {
  while (*path != END) {
    if (*path == UP) {
      digitalWrite(DIR1_PIN, LOW);
      digitalWrite(DIR2_PIN, LOW);
    } else if (*path == DOWN) {
      digitalWrite(DIR1_PIN, HIGH);
      digitalWrite(DIR2_PIN, HIGH);
    } else if (*path == LEFT) {
      digitalWrite(DIR1_PIN, HIGH);
      digitalWrite(DIR2_PIN, LOW);
    } else if (*path == RIGHT) {
      digitalWrite(DIR1_PIN, LOW);
      digitalWrite(DIR2_PIN, HIGH);
    }

    for (int i=0; i<STEPS_PER_SQUARE/2; i++) stepMotor();

    path += 1;
  }
}

/* MUX Survey function. To access each pin:
   1. Loop through the numbers 0-15 in each mux
   2. Select Mux Pin function with pin number
   3. Use AnalogRead to get the value from the pin */
void scanBoard() {
  digitalWrite(LEDB, HIGH);
  for (int square=0; square<64; square++) {
    for (int i = 0; i < 4; i++)
      digitalWrite(SELECT_PINS[i], square & (1<<i) ? HIGH : LOW);
    board[square] = analogRead(COM_PIN);
  }
  digitalWrite(LEDB, LOW);
}

void printBoard() {
  Serial.println("");
  Serial.println("  ABCDEFGH");
  Serial.println("  --------");
  for (int row=7; row>=0; row--) {
    Serial.print(row+1);
    Serial.print('|');
    for (int col=0; col<8; col++)
      Serial.print(board[row*8+col] ? 'X' : '.');
    Serial.print('\n');
  }
}


// const int dirPin1 = 5;
// const int stepPin1 = 4;
// const int dirPin2 = 3;
// const int stepPin2 = 2;
// const int stepsPerRevolution = 200;
// void driveMotors() {
  
//   for (int cnt=0; cnt<1; cnt++) {
//     if (cnt==0){
//       digitalWrite(dirPin1, HIGH); // Set motor 1 to clockwise
//       digitalWrite(dirPin2, HIGH);// Set motor 2 to clockwise
//     }
//     if (cnt==1){
//       digitalWrite(dirPin1, LOW); // Set motor 1 to clockwise
//       digitalWrite(dirPin2, HIGH);// Set motor 2 to clockwise
//     }
//     if (cnt==2){
//       digitalWrite(dirPin2, LOW);// Set motor 2 to clockwise
//     }
//     if (cnt==3){
//       digitalWrite(dirPin1, HIGH); // Set motor 1 to clockwise
//       digitalWrite(dirPin2, LOW);// Set motor 2 to clockwise
//     }

//     Serial.println("Turning magnet on");
//     digitalWrite(LEDR, HIGH);
//     digitalWrite(LEDB, HIGH);
//     digitalWrite(LEDG, HIGH);
    
//     for (int i=0; 1; i++) {
//       if (i%50 == 0) Serial.println(i);
//       digitalWrite(stepPin1, HIGH);
//       digitalWrite(stepPin2, HIGH);
//       delayMicroseconds(10000);
//       digitalWrite(stepPin1, LOW);
//       digitalWrite(stepPin2, LOW);
//       delayMicroseconds(10000);
//     }
//   }
// }
