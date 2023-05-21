#include <WiFi.h>
#include <WiFiUdp.h>

const char *ssid = "drone";
const char *password = "12345678";

WiFiUDP udp; //udp instance
unsigned int localUdpPort = 4210; // local port to listen on

char incomingPacket[255]; // buffer for incoming packets

void setup() {
  Serial.begin(115200);

  // WiFi network as an access point
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  //starting udp
  udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
}

void loop() {
  WiFiClient client = WiFi.softAPgetStationNum();
  // trying to parse packet
  int packetSize = udp.parsePacket();
  if (packetSize) {
    // receive incoming UDP packets
    int len = udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = '\0';
    }
    Serial.printf("UDP packet contents: %s\n", incomingPacket);
  }

  delay(2500);
}
