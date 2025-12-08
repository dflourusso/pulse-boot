#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "secrets.h"

#define RELAY_PIN 0
#define SUCCESS_CHECK_INTERVAL 300000   // 5m between checks
#define FAIL_CHECK_INTERVAL 20000   // 20s between checks
#define FAIL_THRESHOLD 10

int failCount = 0;
WiFiClient client;

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // relé OFF on boot (active LOW)

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected\n");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
    delay(1000);
    return;
  }

  HTTPClient http;
  http.begin(client, endpoint);
  int httpCode = http.GET();
  http.end();

  Serial.printf("HTTP status: %d\n", httpCode);

  if (httpCode == 200) {
    Serial.printf("HTTP OK\n");
    failCount = 0;
  } else {
    failCount++;
    Serial.printf("Fail: %d/%d\n", failCount, FAIL_THRESHOLD);
  }

  if (failCount >= FAIL_THRESHOLD) {
    failCount = 0;
    sendPulse();
  }

  if(failCount == 0) {
    delay(SUCCESS_CHECK_INTERVAL);
  } else {
    delay(FAIL_CHECK_INTERVAL);
  }
}

void sendPulse() {
  Serial.println("Pulsing relay...\n");
  digitalWrite(RELAY_PIN, LOW);
  delay(300);
  digitalWrite(RELAY_PIN, HIGH);
}
