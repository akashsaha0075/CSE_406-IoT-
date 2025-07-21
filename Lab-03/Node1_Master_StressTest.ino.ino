#include <SoftwareSerial.h>

#define RX_PIN D6  // GPIO12
#define TX_PIN D5  // GPIO14

SoftwareSerial softSerial(RX_PIN, TX_PIN);

unsigned long receivedCount = 0;
unsigned long errorCount = 0;

const int testBaudRates[] = {38400}; // more reliable than 115200
const int testMessageSizes[] = {10, 50, 100};
const int testIntervals[] = {0, 10, 100};

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== UART Stress Test Master ===");
  softSerial.begin(38400);
  delay(1000);
}

void loop() {
  for (int b = 0; b < 1; b++) {
    int baud = testBaudRates[b];
    softSerial.begin(baud);
    delay(500);
    Serial.println("\nTesting at baud: " + String(baud));

    for (int s = 0; s < 3; s++) {
      int size = testMessageSizes[s];

      for (int i = 0; i < 3; i++) {
        int interval = testIntervals[i];

        runTest(baud, size, interval);
      }
    }
  }

  while (1); // Stop after all tests
}

void runTest(int baud, int size, int interval) {
  Serial.println("Testing size: " + String(size) + ", interval: " + String(interval));
  int totalMessages = 50;
  receivedCount = 0;
  errorCount = 0;

  unsigned long startTime = millis();

  for (int i = 0; i < totalMessages; i++) {
    // Prepare message
    String msg = "SEQ:" + String(i) + ";";
    while (msg.length() < size) msg += "X";

    // Flush buffer before sending
    while (softSerial.available()) softSerial.read();

    // Send message
    softSerial.println(msg);
    Serial.println("Sent: " + msg);

    // Wait for response
    String echo = waitForResponse(800);
    Serial.println("Received: " + echo);

    // Check response
    if (echo.startsWith("SEQ:" + String(i))) {
      receivedCount++;
    } else {
      errorCount++;
      Serial.println("❌ Mismatch! Sent: " + msg + " | Received: " + echo);
    }

    delay(interval);
  }

  unsigned long duration = millis() - startTime;
  float throughput = (size * receivedCount * 1.0) / (duration / 1000.0); // bytes/sec
  float msgRate = (receivedCount * 1000.0) / duration;
  float errorRate = (errorCount * 100.0) / totalMessages;

  Serial.println("Duration: " + String(duration) + "ms");
  Serial.println("Messages Received: " + String(receivedCount));
  Serial.println("Errors: " + String(errorCount));
  Serial.println("Throughput: " + String(throughput, 2) + " bytes/sec");
  Serial.println("Message Rate: " + String(msgRate, 2) + " msg/sec");
  Serial.println("Error Rate: " + String(errorRate, 2) + "%");
  Serial.println("------------------------------");
}

String waitForResponse(unsigned long timeout) {
  unsigned long start = millis();
  String response = "";

  while (millis() - start < timeout) {
    if (softSerial.available()) {
      response = softSerial.readStringUntil('\n');
      return response;
    }
  }
  return "";
}
