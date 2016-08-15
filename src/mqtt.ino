#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <PubSubClient.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>

// WiFi credentials
const char *ssid = "Galileo";
const char *password = "tortillapapas";

// MQTT parameters
const char *topic = "test";
const char *server = "mqtt.kinton.xyz";
const char *clientName = "ESP8266";

// Handle message arrived via MQTT
void callback(char *topic, byte *payload, unsigned int length) {}

// Clients
WiFiUDP ntpUDP;
WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);
WiFiManager wifiManager;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600 * 2, 60000);

// Connects to the MQTT broker
void connect_mqtt() {
  if (!client.connect(clientName)) {
    Serial.println("MQTT connect failed");
    delay(5000);
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }

  timeClient.begin();
  wifiManager.autoConnect("ESP8266");
}

void loop() {
  if (!client.connected()) {
    connect_mqtt();
    return;
  }

  timeClient.update();

  const char *payload = timeClient.getFormattedTime().c_str();
  if (!client.publish(topic, payload)) {
    Serial.println("Publish failed");
  }

  // ESP.deepSleep(10 * 1000000);
  delay(10000);
}
