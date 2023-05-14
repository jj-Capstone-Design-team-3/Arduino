#include <sMQTTBroker.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

const char* ssid = "Drone_AP";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "1q2w3e4r"; // The password of the Wi-Fi network
const unsigned short mqttPort=1883;

sMQTTBroker broker;
WiFiClient wifiClient;
PubSubClient client(wifiClient);

// Function to handle received messages
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Data: ");
    for (uint8_t i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

// Function for subscribing to MQTT messages in a separate thread
void subscribeTask(void * parameter) {
    client.setServer(WiFi.softAPIP(), mqttPort);
    client.setCallback(callback);
    
    while (true) {
        if (!client.connected()) {
            Serial.println("Attempting MQTT connection...");
            if (client.connect("ESP32_Sub")) {
                Serial.println("connected");
                client.subscribe("outTopic");
            } else {
                Serial.print("failed, rc=");
                Serial.print(client.state());
                Serial.println(" try again in 5 seconds");
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
        } else {
            client.loop();
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void setup()
{
    Serial.begin(115200);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    Serial.print("IP address:\t");
    Serial.println(WiFi.softAPIP());
    broker.init(mqttPort);

    // Create a new task for MQTT subscription
    xTaskCreate(
        subscribeTask,          // Task function
        "SubscribeTask",        // Task name
        10000,                  // Stack size
        NULL,                   // Task input parameter
        1,                      // Task priority
        NULL                    // Task handle
    );
}

void loop()
{
    broker.update();
}
