#include <SoftwareSerial.h>

#define RX_PIN D6  // GPIO12
#define TX_PIN D5  // GPIO14

SoftwareSerial softSerial(RX_PIN, TX_PIN);
int currentBaud = 38400;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== UART Slave Started ===");

  softSerial.begin(currentBaud);
}

void loop() {
  // Optional: allow baud change from Serial Monitor
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    if (cmd.startsWith("BAUD:")) {
      int newBaud = cmd.substring(5).toInt();
      if (newBaud > 0) {
        currentBaud = newBaud;
        softSerial.begin(currentBaud);
        Serial.println("Baud changed to: " + String(currentBaud));
      }
    }
  }

  // Handle incoming UART message
  if (softSerial.available()) {
    String incoming = softSerial.readStringUntil('\n');
    Serial.println("Received: " + incoming);
    softSerial.println(incoming); // echo back to master
    Serial.println("Echoed back.");
  }
}
