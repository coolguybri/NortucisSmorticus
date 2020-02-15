 
#include <SoftwareSerial.h>

#define PIN_BLUETOOTH_RECV 2
#define PIN_BLUETOOTH_SEND 3
#define PIN_BLUETOOTH_POWER 5
#define PIN_BLUETOOTH_ENABLE 4

SoftwareSerial BTserial(PIN_BLUETOOTH_RECV, PIN_BLUETOOTH_SEND); // RX | TX
 
const long baudRate = 38400; 
char c=' ';
boolean NL = true;

void setup() 
{
    Serial.begin(9600);
    Serial.print("Sketch:   ");   
    Serial.println(__FILE__);
    Serial.print("Uploaded: ");   
    Serial.println(__DATE__);
    Serial.println(" ");

    // Init the 2 bluetooth control pins; POWER goes to its VIn, so 
    // flipping that high and low turns it on and off. The enable pin is used 
    // to put the HC-05 in control (at) mode; If enable is high when you turn
    // it on, it starts in command mode.
    pinMode(PIN_BLUETOOTH_POWER, OUTPUT);
    pinMode(PIN_BLUETOOTH_ENABLE, OUTPUT);
    digitalWrite(PIN_BLUETOOTH_POWER, LOW);
    digitalWrite(PIN_BLUETOOTH_ENABLE, LOW);

    //swithToBluetoothNormalMode();
    swithToBluetoothCommandMode();
 
    BTserial.begin(baudRate);  
    Serial.print("Communications started at "); 
    Serial.println(baudRate);
    Serial.println(" ");
}
 
void loop()
{
    // Read all responses from the Bluetooth module and send to the Arduino Serial Monitor
    if (BTserial.available())
    {
        c = BTserial.read();
        Serial.write("received: ");
        Serial.write(c);
        Serial.write(" (");
        Serial.print(String(int(c)));
        Serial.write(")\r\n");
    }
 
    // Read from the Serial Monitor and send to the Bluetooth module
    if (Serial.available())
    {
        c = Serial.read();
        int charcode = c;  
        Serial.write("sent: ");
        Serial.println(String(charcode));
        //Serial.write("\r\n");

         //BTserial.write(c); 

         c = 'a';
         BTserial.write(c); 
         c = 't';
         BTserial.write(c);
         c = 13;
         BTserial.write(c); 
         c = 10;
         BTserial.write(c); 
 
        // Echo the user input to the main window. The ">" character indicates the user entered text.
        //if (NL) { Serial.print(">");  NL = false; }
        //Serial.write(c);
        //if (c==10) { NL = true; }
    }
}

/**
 * Cycle the power on the bluetooth module, starting it back up in normal mode.
 */
void swithToBluetoothNormalMode() {
  // power it down
  digitalWrite(PIN_BLUETOOTH_POWER, LOW);
  delay(100);

  // power it up with the enable pin off.
  digitalWrite(PIN_BLUETOOTH_ENABLE, LOW);
  delay(200);
  digitalWrite(PIN_BLUETOOTH_POWER, HIGH);
  delay(500);
}


/**
 * Cycle the power on the bluetooth module, starting it back up in command mode.
 */
void swithToBluetoothCommandMode() {
  // power it down
  digitalWrite(PIN_BLUETOOTH_POWER, LOW);
  delay(100);

  // power it up with the enable pin on.
  digitalWrite(PIN_BLUETOOTH_ENABLE, HIGH);
  delay(200);
  digitalWrite(PIN_BLUETOOTH_POWER, HIGH);
  delay(500);
}
