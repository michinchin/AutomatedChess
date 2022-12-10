//#include <ArduinoBLE.h>
//#include <path.c>
#include <LiquidCrystal.h>


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
*/

// LED_BUILTIN = Bluetooth Connection Status
// LEDR = Error
// LEDG = Magnet
// LEDB = Scan Board

// Pin Definitions
const int SELECT_PINS[4] = {19, 20, 21, 22}; // A0, A1, A2, A3
const int ENABLE_MUX_PINS[4] = {13, 7, 8, 9}; // D13, D7, D8, D9
const int COM_PIN = 15;
const int MAGNET_PIN = 6;
const int dirPin1 = 4;
const int stepPin1 = 5;
const int dirPin2 = 2;
const int stepPin2 = 3;
const int stepsPerRevolution = 200;
const int LED_ON_TIME = 500;  // Each LED is on for 0.5s
// LCD
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);



void setup() {
  Serial.begin(9600);
  while (!Serial);

  lcd.begin(16, 2);
  lcd.print("hello, world!");

//  setupBLE();
  setupMux();
  setupMagnet();
  setupMotor();
//  currentState = CONNECT;

//  pinMode(LEDR, OUTPUT);
//  pinMode(LEDG, OUTPUT);
//  pinMode(LEDB, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  driveMotors();
}
//
//void setupBLE(){
//  if (!BLE.begin()) {
//    Serial.println("starting Bluetooth® Low Energy failed!");
//    digitalWrite(LEDR, HIGH);
//    while (1);
//  }
//
//  BLE.setLocalName("AutomatedChess");
//  BLE.setAdvertisedService(service);
//  service.addCharacteristic(command);
//  // service.addCharacteristic(currLocCharacteristic);
//  BLE.addService(service);
//  // currLocCharacteristic.writeValue(0);
//  BLE.advertise();
//}

void setupMux() {
  for (int i = 0; i < 4; i++) {
    pinMode(SELECT_PINS[i], OUTPUT);
    digitalWrite(SELECT_PINS[i], LOW);
    pinMode(ENABLE_MUX_PINS[i], OUTPUT);
    digitalWrite(ENABLE_MUX_PINS[i], LOW); //FIXME: check pull up/down on EN
  }
  pinMode(COM_PIN, INPUT);
}

void setupMagnet() {
  pinMode(MAGNET_PIN, OUTPUT);
  digitalWrite(MAGNET_PIN, LOW);
}

void setupMotor() {
  pinMode(dirPin1, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  pinMode(stepPin1, OUTPUT);
  pinMode(stepPin2, OUTPUT);
}

void loop() {
  
  // Wait until device is connected, check every second and blink built-in led.
//  BLEDevice central;
//  while (central = BLE.central(); !central) {
//    digitalWrite(LED_BUILTIN, HIGH);
//    delay(500);
//    digitalWrite(LED_BUILTIN, LOW);
//    delay(500);
//  }
//
//  if (central) {
//    Serial.printf("Connected to central: %s\n", central.address());
//    digitalWrite(LED_BUILTIN, HIGH);  // turn on the LED to indicate the connection
//    while (central.connected()) {
//      scanBoard();
//      waitAndExecuteCommand();
//    }
//    digitalWrite(LED_BUILTIN, LOW);  // turn on the LED to indicate the connection
//    Serial.printf("Disconnected to central: %s\n", central.address());
//  }
}
//
//void waitAndExecuteCommand() {
//  while (!command.written());
//  Serial.println(command.value());
//  lcd.print(command.value());
//
//  // TODO: parse command such that the following are fully qualified by the command.value()
//  int src = 2;
//  int dst = 9;
//
//  // goto src
//  int queue[64];
//  findPath(dst, src, queue);
//
//  for (int i=0; queue[i] != -1; i++) {
//    // Do something
//  }
//}

/* MUX Survey function. To access each pin:
   1. Loop through the numbers 0-15 in each mux
   2. Select Mux Pin function with pin number
   3. Use AnalogRead to get the value from the pin */
//void scanBoard() {
////  digitalWrite(LEDB, HIGH);
//  for (int square=0; square<64; square++) {
//    for (int i = 0; i < 4; i++)
//      digitalWrite(SELECT_PINS[i], square & (1<<i) ? HIGH : LOW);
//    board[square] = analogRead(COM_PIN);
//  }
//  digitalWrite(LEDB, LOW);
//}


//void driveMagnet() {
//  delayMicroseconds(500);
//  digitalWrite(MAGNET_PIN, HIGH);
//  delayMicroseconds(500);
//  digitalWrite(MAGNET_PIN, LOW);
//  }

void driveMotors() {
  
  for (int cnt=0; cnt<4; cnt++) {
    if (cnt==0){
      digitalWrite(dirPin1, HIGH); // Set motor 1 to clockwise
      digitalWrite(dirPin2, HIGH);// Set motor 2 to clockwise
    }
    if (cnt==1){
      digitalWrite(dirPin1, LOW); // Set motor 1 to clockwise
      digitalWrite(dirPin2, HIGH);// Set motor 2 to clockwise
    }
    if (cnt==2){
      digitalWrite(dirPin1, LOW); // Set motor 1 to clockwise
      digitalWrite(dirPin2, LOW);// Set motor 2 to clockwise
    }
    if (cnt==3){
      digitalWrite(dirPin1, HIGH); // Set motor 1 to clockwise
      digitalWrite(dirPin2, LOW);// Set motor 2 to clockwise
    }
    
    Serial.println("Turning magnet on");
    digitalWrite(MAGNET_PIN, HIGH);
    for (int x=0; x < 3*stepsPerRevolution; x++) {
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(stepPin1, HIGH);
      digitalWrite(stepPin2, HIGH);
      Serial.println(stepPin1);
      Serial.println(stepPin2);
      
      delayMicroseconds(1000);
      digitalWrite(LED_BUILTIN, LOW);
      digitalWrite(stepPin1, LOW);
      digitalWrite(stepPin2, LOW);
      delayMicroseconds(1000);
    }  
  }
  Serial.println("MAGNET OFF");  
  digitalWrite(MAGNET_PIN, LOW);
}
