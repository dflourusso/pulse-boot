#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>
#include "secrets.h" // ssid, password and IP

#define RELAY_PIN 0
#define SUCCESS_CHECK_INTERVAL 60000   // 1m between checks
#define FAIL_CHECK_INTERVAL 10000       // 10s between checks
#define FAIL_THRESHOLD 20

int failCount = 0;
IPAddress targetIP(targetIP_array);

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
    Serial.println("Wifi disconnected, trying to connect...");
    delay(1000);
    ESP.restart();
    return;
  }

  // Faz ping para a máquina
  if (Ping.ping(targetIP, 1)) {
    Serial.println("Machine online");
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
