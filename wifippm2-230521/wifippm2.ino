#include <ESP8266WiFi.h>
#include <WiFiUdp.h>  // Include the UDP library
#include "MSP.h"

/* Set these to your desired credentials. */
const char *ssid = "drone";
const char *password = "12345678";

#define CPU_MHZ 80
#define CHANNEL_NUMBER 8  //set the number of channels
#define CHANNEL_DEFAULT_VALUE 1100  //set the default servo value
#define FRAME_LENGTH 22500  //set the PPM frame length in microseconds (1ms = 1000µs)
#define PULSE_LENGTH 300  //set the pulse length
#define onState 0  //set polarity of the pulses: 1 is positive, 0 is negative
#define sigPin 2 //set PPM signal output pin on the arduino
#define DEBUGPIN 3

volatile unsigned long next;
volatile unsigned int ppm_running = 1;

int ppm[CHANNEL_NUMBER];

unsigned int alivecount = 0;

// Create a WiFiUDP object
WiFiUDP udp;

MSP msp;

void inline ppmISR(void){
  static boolean state = true;

  if (state) {  //start pulse
    digitalWrite(sigPin, onState);
    next = next + (PULSE_LENGTH * CPU_MHZ);
    state = false;
    alivecount++;
  } 
  else{  //end pulse and calculate when to start the next pulse
    static byte cur_chan_numb;
    static unsigned int calc_rest;
  
    digitalWrite(sigPin, !onState);
    state = true;

    if(cur_chan_numb >= CHANNEL_NUMBER){
      cur_chan_numb = 0;
      calc_rest = calc_rest + PULSE_LENGTH;// 
      next = next + ((FRAME_LENGTH - calc_rest) * CPU_MHZ);
      calc_rest = 0;
      digitalWrite(DEBUGPIN, !digitalRead(DEBUGPIN));
    }
    else{
      next = next + ((ppm[cur_chan_numb] - PULSE_LENGTH) * CPU_MHZ);
      calc_rest = calc_rest + ppm[cur_chan_numb];
      cur_chan_numb++;
    }     
  }
  timer0_write(next);
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  udp.begin(4210);
  pinMode(sigPin,OUTPUT);
  digitalWrite(sigPin, !onState); //set the PPM signal pin to the default state (off)
  pinMode(DEBUGPIN,OUTPUT);
  digitalWrite(DEBUGPIN, !onState); //set the PPM signal pin to the default state (off)

  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(ppmISR);
  next=ESP.getCycleCount()+1000;
  timer0_write(next);
  for(int i=0; i<CHANNEL_NUMBER; i++){
    ppm[i]= CHANNEL_DEFAULT_VALUE;
  }
  interrupts();
  msp.begin(Serial);
  udp.beginPacket("192.168.4.1", 4210);
  udp.write("Hello AP");
  udp.endPacket();
}

unsigned long time_now = 0;

void loop() {
  if(alivecount>1000){
    for(int i=0; i<4;i++){
      ppm[i]=900;
    }
    for(int i=4; i<8;i++){
      ppm[i]=1100;
    }
  }
  unsigned long currentTime = millis();
  // Send PPM data to the drone every second
  if (currentTime - time_now >= 1000) {
    udp.beginPacket("192.168.4.1", 4210);
    for(int i=0; i<CHANNEL_NUMBER; i++){
      String ppmData = String(ppm[i]);
      udp.print(ppmData);
      if (i < CHANNEL_NUMBER - 1) {
        udp.print(",");  // Separate data with comma
      }
    }
    udp.endPacket();
    time_now = currentTime;
  }
  yield();
}
