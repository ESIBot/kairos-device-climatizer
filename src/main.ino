#include "kairos_sun/kairos_sun.hpp"

#include <ArduinoJson.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <PubSubClient.h>
#include <RTClib.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <Wire.h>

using namespace ArduinoJson::Internals;

// Globals
const char *mqtt_topic = "8d6fd306-e778-4e63-a2b5-2f6a9ada13ca/sun";
const char *topics[] = {"8d6fd306-e778-4e63-a2b5-2f6a9ada13ca/"
                        "719a109c-1aac-4b5a-b749-2b1587d6be53"};
const char *mqtt_server = "mqtt.kinton.xyz";
const int mqtt_port = 1884;
const char *mqtt_user = "719a109c-1aac-4b5a-b749-2b1587d6be53";
const char *mqtt_password =
    "62f71f8ed6860c18abbbff6429b963ed444cee1f1d4d9b9aa165aab1a7ab9e0b";
const uint8_t RELAY = 5;
const int START_HOUR = 8;
const int START_MINUTE = 0;
const int INTERVAL = 8 * 60;
const int UPDATE_INTERVAL = 10000;
const int JSON_BUFFER_LENGTH = 200;

// MQTT callback
void callback(char *topic, byte *payload, unsigned int length);

// Clients
WiFiUDP ntpUDP;
WiFiClient wifiClient;
WiFiManager wifiManager;
NTPClient timeClient(ntpUDP, "163.172.173.19", 3600 * 2, 60 * 60 * 1000);
KairosSun Sun(RELAY, START_HOUR, START_MINUTE, INTERVAL, UPDATE_INTERVAL);
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
  Sun.SetMode(on);

  // Set pin mode
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, HIGH);

  Wire.begin(12, 14);
  RTC.begin();

  sync_time();

  Serial.println("Setup finished");
}

void loop() {
  // Connect to MQTT broker
  if (!mqtt_client.connected()) {
    connect_mqtt();
    return;
  }

  // Call Update often to ensure the light is in the desired state
  bool changed = Sun.Update();

  // Notify via MQTT the light status
  if (changed && Sun.State()) {
    Serial.println("Light ON");

    if (!mqtt_client.publish(
            mqtt_topic, "{\"device\": \"climatizer\", \"light\": \"on\"}")) {
      Serial.println("Publish failed");
    }
  }
  if (changed && !Sun.State()) {
    Serial.println("Light OFF");

    if (!mqtt_client.publish(
            mqtt_topic, "{\"device\": \"climatizer\", \"light\": \"off\"}")) {
      Serial.println("Publish failed");
    }
  }

  mqtt_client.loop();
}

// Connects to the MQTT broker
void connect_mqtt() {
  if (!mqtt_client.connect("climatizer", mqtt_user, mqtt_password)) {
    Serial.println("MQTT connect failed");
    return;
  }

  Serial.println("Connected to Kinton");

  for (int i = 0; i < (sizeof(topics) / sizeof(*topics)); i++) {
    mqtt_client.subscribe(topics[i]);
    Serial.printf("Suscribed to %s\n", topics[i]);
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

// Handle message received via MQTT
void callback(char *topic, byte *payload, unsigned int length) {
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject &root = jsonBuffer.parseObject((char *)payload);
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }

  const char *element = root["element"];

  if (strcmp(element, "sun") == 0) {
    const char *action = root["action"];

    if (strncmp(action, "set", strlen("set_status")) == 0) {
      const char *value = root["value"];

      if (strcmp(value, "on") == 0) {
        Sun.SetMode(on);
      } else if (strcmp(value, "off") == 0) {
        Sun.SetMode(off);
      } else if (strcmp(value, "timer") == 0) {
        Sun.SetMode(timer);
      } else {
        Serial.println("Invalid value");
      }
    } else {
      Serial.println("Invalid action");
    }
  } else {
    Serial.println("Invalid element");
  }
}
