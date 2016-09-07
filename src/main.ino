#include "kairos_sun/kairos_sun.hpp"

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <PubSubClient.h>
#include <RTClib.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <Wire.h>

// Globals
const char *mqtt_topic = "8d6fd306-e778-4e63-a2b5-2f6a9ada13ca/sun>";
const char *mqtt_server = "mqtt.kinton.xyz";
const int mqtt_port = 1884;
const char *mqtt_user = "719a109c-1aac-4b5a-b749-2b1587d6be53";
const char *mqtt_password =
    "62f71f8ed6860c18abbbff6429b963ed444cee1f1d4d9b9aa165aab1a7ab9e0b";
const uint8_t RELAY = 5;
const int START_HOUR = 8;
const int START_MINUTE = 0;
const int INTERVAL = 8 * 60;

// Handle message received via MQTT
void callback(char *topic, byte *payload, unsigned int length) {}

// Clients
WiFiUDP ntpUDP;
WiFiClient wifiClient;
WiFiManager wifiManager;
NTPClient timeClient(ntpUDP, "163.172.173.19", 3600 * 2, 60 * 60 * 1000);
KairosSun Sun(RELAY, START_HOUR, START_MINUTE, INTERVAL);
PubSubClient mqtt_client(mqtt_server, mqtt_port, callback, wifiClient);
RTC_DS1307 RTC;

void setup() {
  // Set serial port
  Serial.begin(115200);
  while (!Serial) {
  }

  // Connect to WiFi
  wifiManager.autoConnect("KAIROS Climatizer");

  // Start NTP service
  timeClient.begin();
  Sun.SetRTC(RTC);

  // Set pin mode
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, HIGH);

  Wire.begin(12, 14);
  RTC.begin();

  sync_time();

  Serial.println("Setup finished");
}

void loop() {
  // Call Update often to ensure the light is in the desired state
  bool on = Sun.Update();

  // Connect to MQTT broker
  if (!mqtt_client.connected()) {
    connect_mqtt();
    return;
  }

  // Notify via MQTT the light status
  if (on) {
    if (!mqtt_client.publish(
            mqtt_topic, "{\"device\": \"climatizer\", \"light\": \"on\"}")) {
      Serial.println("Publish failed");
    }
  } else {
    if (!mqtt_client.publish(
            mqtt_topic, "{\"device\": \"climatizer\", \"light\": \"off\"}")) {
      Serial.println("Publish failed");
    }
  }

  delay(10000);
}

// Connects to the MQTT broker
void connect_mqtt() {
  if (!mqtt_client.connect("climatizer", mqtt_user, mqtt_password)) {
    Serial.println("MQTT connect failed");
    delay(5000);
  }
}

// Sync local time using NTP
void sync_time() {
  if (timeClient.forceUpdate()) {
    DateTime now = DateTime(timeClient.getEpochTime());
    RTC.adjust(now);
  } else {
    Serial.println("Error syncing time");
    return;
  }
}
