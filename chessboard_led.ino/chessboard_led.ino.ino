#include <ArduinoBLE.h>

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

// MUX Pin Definitions //
const int selectPins[4] = {19, 20, 21, 22}; // A0, A1, A2, A3
const int com_output = 15; // Connect signal pin to 15
const int enable_mux_pins[4] = {13, 7, 8, 9}; // D13, D7, D8, D9
const int magnetPin = 6;

const int LED_ON_TIME = 500;  // Each LED is on for 0.5s
int col_ind = 0;
int row_ind = 0;
int mux_num = 0;
int magnetOn = 0;
// Define the reed sensor matrix, numbered vertically
int reed[8][8] = {
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 1, 1, 1, 1, 1, 1, 1, 1 },
  { 1, 1, 1, 1, 1, 1, 1, 1 },
  { 1, 1, 1, 1, 1, 1, 1, 1 },
  { 1, 1, 1, 1, 1, 1, 1, 1 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 }
};

// BLE Setup

BLEService periphService("4400b98a-0b70-4253-b250-d60e21f0f224");  // BLE  Service
BLEByteCharacteristic currLocCharacteristic("d7a16eff-1ee7-4344-a3d2-a8203d97d75c", BLERead | BLEWrite);
BLEByteCharacteristic destLocCharacteristic("596a72cf-7acc-4181-a9d1-dbf30db2aa7b", BLERead | BLEWrite);

void setup() {
  Serial.begin(9600);
  // while (!Serial);
  setLEDPins();
  setupBLE();
  // Set up the MUX I/O
  muxSetup();
  // Set up the magnet I/O
  magSetup();
}

void loop() {
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, HIGH);  // turn on the LED to indicate the connection

    // while the central is still connected to peripheral:
    while (central.connected()) {
      testBLE();
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
    turnOffLEDS();
  }

  // If a move has been completed, boolean move_complete == true
  // Call functions to survey the mux
//  muxSurvey();
//  magControl();
}

void setupBLE(){
   // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");

    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Arduino33BLE");
  BLE.setAdvertisedService(periphService);

  // add the characteristic to the service
  periphService.addCharacteristic(currLocCharacteristic);
  periphService.addCharacteristic(destLocCharacteristic);

  // add service
  BLE.addService(periphService);

  // set the initial value for the characteristic:
  currLocCharacteristic.writeValue(0);
  destLocCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("BLE LED Peripheral");
}

void turnOffLEDS() {
  digitalWrite(LED_BUILTIN, LOW);  // when the central disconnects, turn off the LED
  digitalWrite(LEDR, HIGH);        // will turn the LED off
  digitalWrite(LEDG, HIGH);        // will turn the LED off
}
/*
  Testing LEDs using BLE
*/
void testBLE() {
  // if the remote device wrote to the characteristic,
  // use the value to control the LED:
  if (currLocCharacteristic.written()) {
    Serial.println(sizeof(currLocCharacteristic.value())); // use this curr loc value to find path
  }
  if (destLocCharacteristic.written()) {
    Serial.println(sizeof(destLocCharacteristic.value())); // use this dest loc value to find path
  }
}

void setLEDPins() {
  // set LED's pin to output mode
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, LOW);  // when the central disconnects, turn off the LED
  digitalWrite(LEDR, HIGH);        // will turn the LED off
  digitalWrite(LEDG, HIGH);        // will turn the LED off
  digitalWrite(LEDB, HIGH);        // will turn the LED off
}

/*
  Mux Initialize Function. To be used in void setup()
*/
void muxSetup() {
  for (int i = 0; i < 4; i++) {
    // Set up select pins as outputs
    pinMode(selectPins[i], OUTPUT);
    digitalWrite(selectPins[i], LOW);

        // Set up enable pins as outputs
        pinMode(enable_mux_pins[i], OUTPUT);
        digitalWrite(enable_mux_pins[i], LOW); //FIXME: check pull up/down on EN
    }
    pinMode(com_output, INPUT); // Set up COM as an input
    
    // Print the header:
    Serial.println("Y0\tY1\tY2\tY3\tY4\tY5\tY6\tY7");
    Serial.println("---\t---\t---\t---\t---\t---\t---\t---");
}

/*
  Magnet Initialize Function. To be used in void setup()
*/
void magSetup() {
  pinMode(magnetPin, OUTPUT);
  digitalWrite(magnetPin, LOW);
  Serial.println("Magnet initialization complete.");
}

void magControl() {
  while(1) {
    magnetOn = Serial.read();
    
    // if magnet is turned on, turn on all LEDS
    if (magnetOn != -1) { 
      Serial.println("Serial input received");
      Serial.println(magnetOn);
      digitalWrite(LEDR, HIGH);
      digitalWrite(LEDG, LOW);     
      digitalWrite(LEDB, LOW); 
      delay(1000);                  // waits for a second  
      // Drive pin 6 HIGH
      digitalWrite(magnetPin, LOW);
      Serial.println("Magnet ON");   
    } else {
      digitalWrite(LEDR, LOW);
      digitalWrite(LEDG, HIGH);     
      digitalWrite(LEDB, HIGH); 
      delay(1000);                  // waits for a second  
      // Drive pin 6 LOW
      digitalWrite(magnetPin, LOW); 
      Serial.println("Magnet off");  
    }
  }
}

/*
    MUX Survey function. To access each pin:
    1. Loop through the numbers 0-15 in each mux
    2. Select Mux Pin function with pin number 
    3. Use AnalogRead to get the value from the pin
*/
void muxSurvey() {
  // TODO: Figure out how to index across the 4 muxes
  for (int pin = 0; pin < 64; pin++) {
    selectMuxPin(pin);
    col_ind = pin / 8;   // calculate the column number
    row_ind = pin % 8;   // calculate the row number
    mux_num = pin / 16;  // calculate the mux number
    reed[row_ind][col_ind] = analogRead(com_output);
    Serial.print(String(reed[row_ind][col_ind]) + "\t");
  }
}


/* 
    Function to set the select pins for each mux 
*/
void selectMuxPin(byte pin) {
  if (pin > 63) return;  // pin is out of scope
  // For each pin, calculate the corresponding binary
  // toggle select pins output to match the binary value
  for (int i = 0; i < 4; i++) {
    if (pin & (1 << i))
      digitalWrite(selectPins[i], HIGH);
    else
      digitalWrite(selectPins[i], LOW);
  }
}
