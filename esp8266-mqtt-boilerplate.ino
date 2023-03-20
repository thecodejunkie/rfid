#include <ESP8266WiFi.h>
#include "config.h"
#include <PubSubClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() {
  Serial.begin(115200);
  Serial.println();

  connectToWifi(ssid, wifiPassword);
  mqttClient.setServer(mqttHostname, 1883);

  ArduinoOTA.setHostname(ostHostname);
  ArduinoOTA.setPassword(ostPassword);

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });

  ArduinoOTA.begin();
}

void loop() {
  if (!mqttClient.connected()) {
    connectToMqtt();
  }

  ArduinoOTA.handle();
  mqttClient.loop();
}

void connectToWifi(char ssid[], char password[]) {
  WiFi.begin(ssid, password);

  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected");
}

void callback(char topic[], byte *payload, unsigned int length) {
  char message[length];

  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }

  Serial.println(message);

  if (strcmp(topic, mqttTopic) == 0) {
    Serial.println("Topic match!");
  }

  if (strcmp(message, "1") == 0) {
    Serial.println("Message match!");
  }
}

void connectToMqtt() {
  while (!mqttClient.connected()) {
    Serial.println("Attempting MQTT connection...");

    if (mqttClient.connect(mqttClientName, mqttUsername, mqttPassword)) {
      Serial.println("connected");

      mqttClient.subscribe(mqttTopic);
      mqttClient.setCallback(callback);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");

      delay(5000);
    }
  }
}
