#include "kairos_sun/kairos_sun.hpp"

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <PubSubClient.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>

// Globals
const char *mqtt_topic = "<FLEET_KEY>/<TOPIC>";
const char *mqtt_server = "mqtt.kinton.xyz";
const int mqtt_port = 1884;
const char *mqtt_user = "<DEVICE UUID>";
const char *mqtt_password = "<DEVICE SECRET>";
const uint8_t RELAY = 5;
const int START_HOUR = 8;
const int START_MINUTE = 0;
const int INTERVAL = 7 * 60;

// Handle message received via MQTT
void callback(char *topic, byte *payload, unsigned int length) {}

// Clients
WiFiUDP ntpUDP;
WiFiClient wifiClient;
WiFiManager wifiManager;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600 * 2, 60 * 60 * 1000);
KairosSun Sun(RELAY, START_HOUR, START_MINUTE, INTERVAL);
PubSubClient mqtt_client(mqtt_server, mqtt_port, callback, wifiClient);

// Connects to the MQTT broker
void connect_mqtt() {
  if (!mqtt_client.connect("climatizer", mqtt_user, mqtt_password)) {
    Serial.println("MQTT connect failed");
    delay(5000);
  }
}

void setup() {
  // Set serial port
  Serial.begin(115200);
  while (!Serial) {
  }

  // Connect to WiFi
  wifiManager.autoConnect("KAIROS Climatizer");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Start NTP service
  timeClient.begin();
  Sun.SetNTPClient(&timeClient);

  // Set pin mode
  pinMode(RELAY, OUTPUT);

  Serial.println("Setup finished");
}

void loop() {
  // Call Update often to ensure the light is in the desired state
  bool on = Sun.Update();

  // Call Update often to ensure the clock is right
  timeClient.update();

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
