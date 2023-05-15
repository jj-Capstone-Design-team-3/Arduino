int FlexPin = 34; // 센서값을 읽기 위해 아날로그핀 0번을 FlexPin에 지정한다.

void setup()
{
Serial.begin(115200); // 센서값을 읽기 위해 시리얼 모니터를 사용할 것을 설정.
}

void loop()
{
int FlexVal; // 센서값을 저장할 변수
FlexVal = analogRead(FlexPin); // 아날로그를 입력 받음 (0~1023)

Serial.print("sensor: "); // sensor: 라는 텍스트를 프린트한다.
Serial.println(FlexVal); // println은 줄바꿈 명령이다. flexVal의 값을 출력한다.

delay(500);
}