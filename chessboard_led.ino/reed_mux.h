/*
    File to specify Arduino interaction with the reed sensor mux chip. 

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
*/

// Pin Definitions //
const int selectPins[4] = {19, 20, 21, 22}; // A0, A1, A2, A3
const int com_output = 15; // Connect signal pin to 15
const int enable_mux_pins[4] = {13, 7, 8, 9}; // D13, D7, D8, D9

const int LED_ON_TIME = 500; // Each LED is on for 0.5s
int col_ind = 0;
int row_ind = 0;
// Define the reed sensor matrix, numbered vertically
int reed[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
};

void setup() {
    Serial.begin(9600); // Initialize the Serial Port
    
    for (int i=0; i<4; i++) {
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

void loop() {
    /*
        To access each pin:
        1. Loop through the numbers 0-15 in each mux
        2. Select Mux Pin function with pin number 
        3. Use AnalogRead to get the value from the pin
    */
    // TODO: Figure out how to index across the 4 muxes 
    for (int pin=0; pin < 64; pin++) {
        selectMuxPin(pin);
        col_ind = pin / 8; // calculate the column number  
        row_ind = pin % 8; // calculate the row number 
        reed1[row_ind][col_ind] = AnalogRead(com_output);
        Serial.print(String(reed1[row_ind][col_ind]) + "\t");
    }
}

/* 
    Funtion to set the select pins for each mux 
*/
void selectMuxPin(byte pin) {
    if (pin > 63 || pin < 0) return // pin is out of scope
    // For each pin, calculate the corresponding binary
    // toggle select pins output to match the binary value 
    for (int i = 0; i < 4; i++) {
        if (pin & (1<<i))
            digitalWrite(selectPins[i], HIGH);
        else 
            digitalWrite(selectPins[i], LOW);
    }
}
