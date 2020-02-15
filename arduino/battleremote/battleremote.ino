// Import external libraries
#include <Adafruit_ssd1306syp.h>
#include "SoftwareSerial.h"
#include <Servo.h>

// Constants: I/O Pins
#define PIN_BLUETOOTH_RECV 2
#define PIN_BLUETOOTH_SEND 3
#define PIN_BLUETOOTH_POWER 5
#define PIN_BLUETOOTH_ENABLE 4
#define PIN_LED            10
#define PIN_I2C_SDA        A4
#define PIN_I2C_SCL        A5
#define PIN_JOYSTICK_X     A6
#define PIN_JOYSTICK_Y     A7

// More Constants
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// The various states of our bluetooth connection.
enum BluetoothState {
  BLUETOOTH_DISCONNECTED,
  BLUETOOTH_CONNECTED,
  BLUETOOTH_ABANDONDED
};


// Global Variables: Run state
unsigned long startTime = 0; 
const char build_timestamp[] =  __DATE__ " " __TIME__;

// Global Variables: the OLED Display, connected via I2C interface
Adafruit_ssd1306syp display(PIN_I2C_SDA, PIN_I2C_SCL);

// Global Variables: motor control
int velocity = 100;  

// Global Variables: Bluetooth command Queue
SoftwareSerial bluetooth(PIN_BLUETOOTH_RECV, PIN_BLUETOOTH_SEND);
BluetoothState bluetoothState = BLUETOOTH_DISCONNECTED;
char command = 'S';
char prevCommand = 'A';
unsigned long timeLastCommand = 0;  //Stores the time when the last command was received from the phone

/*** DISPLAY ***/


/**
 * Main output for status while in main sequence. This is called once per loop.
 */
void displayStatus(String line1, String line2, String line3, String line4) {  
  display.clear();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0,2);
  display.println(line1);

  // Fun little animation to prove that we are not locked up.
  const int circleRadius = 5;
  const int circleOffset = 0;
  const int maxRight = (SCREEN_WIDTH - 1) - circleRadius;
  const int maxLeft = 82;
  int numFrames = maxRight - maxLeft;
  int numFramesDouble = numFrames * 2;
  int timePerFrame = 3000 / numFramesDouble;
  int currentFrame = (millis() / timePerFrame) % numFramesDouble;
  int circleCenterX = (currentFrame < numFrames) ? (maxRight - currentFrame) : (maxLeft + (currentFrame - numFrames));
  display.drawCircle(circleCenterX, circleRadius + circleOffset, circleRadius, WHITE);
  
  display.setCursor(0,20);
  display.println(line2);
    
  display.setCursor(0,30);
  display.println(line3);

  display.setCursor(0,40);
  display.println(line4);

  // Print the timestamp of when we built this code.
  const int stampHeight = 10;
  int beginStamp = 55;
  display.drawLine(0, beginStamp, SCREEN_WIDTH - 1, beginStamp, WHITE); // top line
  display.drawLine(0, beginStamp, 0, beginStamp + stampHeight, WHITE); // left line
  display.drawLine(SCREEN_WIDTH - 1, beginStamp, SCREEN_WIDTH - 1, beginStamp + stampHeight, WHITE); // right line
  display.setCursor(4, beginStamp + 2);
  display.println(build_timestamp);

  display.update();
}

/**
 * Used for error messages to the OLED display.
 */
void displayMessage(String line1, String line2) {
  display.clear();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  display.setCursor(0,2);
  display.println(line1);
  
  display.setCursor(0,34);
  display.println(line2);
  
  display.update();
}


int bluetoothEnabled = 0;
String lastResponse;

/**
 * Entrypoint: called once when the program first starts, just to initialize all the sub-components.
 */
void setup() {  
  
  // Record what time we started.
  startTime = millis();
  
  // Init the serial line; important for debug messages back to the Arduino Serial Monitor, so you can plug
  // the robot into the USB port, and get real time debug messages.
  // Make sure you set the baudrate at 9600 in Serial Monitor as well.
  Serial.begin(9600);
  Serial.println("setup start...");

  // Init LED pin, and initially set it to on.
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);

  // Init Joystick.
  pinMode(PIN_JOYSTICK_X, INPUT);
  pinMode(PIN_JOYSTICK_Y, INPUT);

  // Init the serial pipe to the bluetooth receiver. NOTE: this can go faster by configuring the HC-05
  // with an AT command. But is that necessary?
  pinMode(PIN_BLUETOOTH_POWER, OUTPUT);
  pinMode(PIN_BLUETOOTH_ENABLE, OUTPUT);
  digitalWrite(PIN_BLUETOOTH_POWER, LOW);
  digitalWrite(PIN_BLUETOOTH_ENABLE, LOW);
  bluetoothEnabled = 0;

  //swithToBluetoothCommandMode();
  bluetooth.begin(38400);
  
  // Init the OLED display.
  delay(1000);
  display.initialize();

  // Init the rest of our internal state.
  Serial.println("setup end");
}


/**
 * Main Loop: called over and over again as the robot runs, 
 */
void loop() {
  
  int now = millis();
  char c;

  // Bluetooth testing. 
  /*if (!bluetoothEnabled && ((now - startTime) > 5000)) {
    swithToBluetoothNormalMode();
    bluetoothEnabled = 1;
  } else if (bluetoothEnabled == 1 && ((now - startTime) > 10000)) {
    swithToBluetoothOffMode();
    bluetoothEnabled = 2;
  } else if (bluetoothEnabled == 2 && ((now - startTime) > 15000)) {
    swithToBluetoothCommandMode();
    bluetoothEnabled = 3;
  } else if (bluetoothEnabled == 3 && ((now - startTime) > 30000)) {
    swithToBluetoothOffMode();
    bluetoothEnabled = 4;
  } */


     // Read from the Bluetooth module and send to the Arduino Serial Monitor
    if (bluetooth.available())
    {
        c = bluetooth.read();
        Serial.write("received: ");
        Serial.write(c);
        Serial.write(" (");
        Serial.print(String(int(c)));
        Serial.write(")\r\n");
    }

  if (!bluetoothEnabled && ((now - startTime) > 3000)) { 
    Serial.println("cmd mode start...");
    //swithToBluetoothCommandMode();
    Serial.println("cmd mode started");

    //bluetooth.begin(38400);
    
      c = 'a';
     bluetooth.write(c); 
      c = 't';
     bluetooth.write(c);
     c = 13;
     bluetooth.write(c); 
      c = 10;
     bluetooth.write(c);
     Serial.println("cmd sent");
     
    //String response = readLineFromBluetooth();
    //lastResponse = response;

    bluetoothEnabled = 1;
  }
    
  
  // Update the LED screen with our current state.
  bool connected = (bluetoothState == BLUETOOTH_CONNECTED);
  int upSecs = (millis() - startTime) / 1000;
  displayStatus(
    connected ? F("CONNECTED") : F("DISCONNECTED"),
    "runtime: " + String(upSecs) + ", bstate=" + String(bluetoothEnabled),
    "stick: " + String(analogRead(PIN_JOYSTICK_X)) + "/" + String(analogRead(PIN_JOYSTICK_Y)),
    "objective: " + String(lastResponse));
}


String readLineFromBluetooth() {
  int startTime = millis();
  int endTime = startTime + 10000;
  char readbuffer[100];
  
  int curr = 0;
  bool readComplete = false;
  while (!readComplete && (millis() < endTime)) {
    //if (bluetooth.available() > 0) {

    int c = bluetooth.read();
    if (c == -1) {
      Serial.print(".");
    } else {
      Serial.println("\nbluetooth read: " + String(c));
      readbuffer[curr++] = c;
    }
    

     /* while (curr < 100) {
        int c = readbuffer[curr++] = bluetooth.read();
        if (c == -1) {
          readComplete = true;
          break;
        }
      } */
   // } else {
    //  Serial.println("nothing on da bluetooth, boss");
    //}
  }
  readbuffer[curr++] = 0; 
  
  /*while (bluetooth.available() > 0) {
    int c = readbuffer[curr++] = bluetooth.read();
    if (c == -1)
    break;
  }
  readbuffer[curr++] = 0; */

  String ret = String(readbuffer);
  Serial.println("bluetooth read complete with: " + ret);
  return ret;
}


void swithToBluetoothCommandMode() {
  digitalWrite(PIN_BLUETOOTH_POWER, LOW);
  delay(100);
  digitalWrite(PIN_BLUETOOTH_ENABLE, HIGH);
  delay(200);
  digitalWrite(PIN_BLUETOOTH_POWER, HIGH);
  delay(500);
}

void swithToBluetoothNormalMode() {
  digitalWrite(PIN_BLUETOOTH_POWER, LOW);
  delay(100);
  digitalWrite(PIN_BLUETOOTH_ENABLE, LOW);
  delay(200);
  digitalWrite(PIN_BLUETOOTH_POWER, HIGH);
  delay(500);
}

void swithToBluetoothOffMode() {
  digitalWrite(PIN_BLUETOOTH_POWER, LOW);
  delay(100);
}
