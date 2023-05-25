/*
 * WebSocketClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>

#include <WebSocketsClient.h>


WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

int ppm[8] = {900,900,900,900,1100,1100,1100,1100};
int oldppm[8] = {0,0,0,0,0,0,0,0};

#define USE_SERIAL Serial

void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
	const uint8_t* src = (const uint8_t*) mem;
	USE_SERIAL.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
	for(uint32_t i = 0; i < len; i++) {
		if(i % cols == 0) {
			USE_SERIAL.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
		}
		USE_SERIAL.printf("%02X ", *src);
		src++;
	}
	USE_SERIAL.printf("\n");
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

	switch(type) {
		case WStype_DISCONNECTED:
			USE_SERIAL.printf("[WSc] Disconnected!\n");
			break;
		case WStype_CONNECTED:
			USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);

			// send message to server when Connected
			webSocket.sendTXT("Connected");
			break;
		case WStype_TEXT:
			USE_SERIAL.printf("[WSc] get text: %s\n", payload);

			// send message to server
			// webSocket.sendTXT("message here");
			break;
		case WStype_BIN:
			USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
			hexdump(payload, length);
      ppm[payload[0]]=(payload[1]<<8)+payload[2];

			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
		case WStype_ERROR:			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
	}

}

void setup() {
	// USE_SERIAL.begin(921600);
	USE_SERIAL.begin(115200);

	//Serial.setDebugOutput(true);
	USE_SERIAL.setDebugOutput(true);

	USE_SERIAL.println();
	USE_SERIAL.println();
	USE_SERIAL.println();

	for(uint8_t t = 4; t > 0; t--) {
		USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
		USE_SERIAL.flush();
		delay(1000);
	}

	WiFiMulti.addAP("WifiPPM", "Wifi_PPM");

	//WiFi.disconnect();
	while(WiFiMulti.run() != WL_CONNECTED) {
		delay(100);
	}

	// server address, port and URL
	webSocket.begin("192.168.4.1", 81, "/");

	// event handler
	webSocket.onEvent(webSocketEvent);

	// use HTTP Basic Authorization this is optional remove if not needed
	webSocket.setAuthorization("user", "Password");

	// try ever 5000 again if connection has failed
	webSocket.setReconnectInterval(5000);

}

void loop() {
	webSocket.loop();
  //String message = "";
  if(Serial.available()){
    int a = Serial.parseInt(); // Parse the first integer
    if (Serial.read() == ',') { // Expect a comma separator
      int b = Serial.parseInt(); // Parse the second integer
      // Assuming ppm[0] is defined somewhere in your code
      uint16_t sendppm[1]; // or however many elements
      sendppm[0] = b; // or whatever value

      // Create and populate sendframe
      uint8_t sendframe[3];
      sendframe[0] = a; //0 ~ 3
      sendframe[1] = sendppm[0] >> 8;
      sendframe[2] = sendppm[0];
      webSocket.sendBIN(sendframe, 3);
      ppm[a] = sendppm[0];
      //message = Serial.readStringUntil('\r');
      //webSocket.sendTXT(message);
    }
  }
  for(int i=0;i<8;i++){
    oldppm[i]=ppm[i];
    uint8_t sendframe[3];
    sendframe[0]=i;
    sendframe[1]=ppm[i]>>8;
    sendframe[2]=ppm[i];
    if (webSocket.isConnected()) {
      webSocket.sendBIN(sendframe, 3);
    }
  }
}
