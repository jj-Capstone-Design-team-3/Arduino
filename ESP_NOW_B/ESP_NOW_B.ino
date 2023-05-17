#include <esp_now.h>
#include <WiFi.h>
// 데이터를 받고 보내는 ESP
// 상대방의 MAC 주소
uint8_t peerAddress[] = {0x78, 0x21, 0x84, 0xE0, 0xE5, 0x00};

struct SensorData {
  String text;
  float sensor_1[6];
};

struct DroneSensor{
  float motor[4];
};

// 콜백 함수
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int len) {
  SensorData receivedSensorData;
  memcpy(&receivedSensorData, data, sizeof(SensorData));
  Serial.print("Received text >> ");
  Serial.println(receivedSensorData.text);
  Serial.print("Received sensor >> [");
  for (int i = 0; i < 6; i++) {
    Serial.print(receivedSensorData.sensor_1[i]);
    if (i < 5) Serial.print(", ");
    else Serial.print("]");
  }
  Serial.println();

  DroneSensor droneSensor;
  for(int i=0;i<4;i++){
    long randomValue = random(1000 * 100, 2000 * 100 + 1); // 1000.00 ~ 2000.00 사이의 랜덤 정수 생성
    droneSensor.motor[i] = randomValue / 100.0; // 정수를 100으로 나누어 float 값으로 변환
  }
  uint8_t SendData[sizeof(DroneSensor)];
  memcpy(SendData, &droneSensor, sizeof(DroneSensor));
  esp_err_t result = esp_now_send(peerAddress, SendData, sizeof(SendData));
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0)); // 랜덤한 센서값을 표시
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // ESP-NOW 초기화
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // 콜백 함수 등록
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);

  // 피어 정보 구조체 초기화
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, peerAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // 피어 추가
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  // 루프에서는 아무 작업도 수행하지 않습니다.
}
