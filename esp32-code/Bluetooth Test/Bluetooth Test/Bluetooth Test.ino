/*
  ESP8266 Blink by Simon Peter
  Blink the blue LED on the ESP-01 module
  This example code is in the public domain

  The blue LED on the ESP-01 module is connected to GPIO1
  (which is also the TXD pin; so we cannot use Serial.print() at the same time)

  Note that this sketch uses LED_BUILTIN to find the pin with the internal LED
*/

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);  // Initialize the LED_BUILTIN pin as an output
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {  // Check if there is new data available in the serial buffer
    String incomingData = Serial.readString();  // Read the incoming data from the serial buffer
    Serial.println("Received data: " + incomingData);  // Print the incoming data to the serial console
  }
  
  digitalWrite(LED_BUILTIN, LOW);  // Turn the LED on (Note that LOW is the voltage level
  Serial.println("B0F255");
  // but actually the LED is on; this is because
  // it is active low on the ESP-01)
  delay(1000);                      // Wait for a second
  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  Serial.println("B255F0");
  delay(2000);                      // Wait for two seconds (to demonstrate the active low LED)
}

