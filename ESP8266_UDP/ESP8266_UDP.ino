#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char *ssid     = "drone";
const char *password = "12345678";
const char *udpAddress = "192.168.4.1"; // Replace with your AP's IP address
const unsigned int udpPort = 4210; // Replace with your desired UDP port

WiFiUDP Udp;

void setup() {
  Serial.begin(115200);
  delay(10);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void loop() {
  Udp.beginPacket(udpAddress, udpPort);
  Udp.write("Hello AP");
  Udp.endPacket();
  delay(1000);
}
