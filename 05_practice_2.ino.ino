int ledPin = 7;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // 1초 동안 켜기
  digitalWrite(ledPin, LOW);   // LED ON (회로 연결에 따라 HIGH/LOW 반대일 수 있음)
  delay(1000);

  // 5번 깜빡이기 (총 1초)
  for (int i = 0; i < 5; i++) {
    digitalWrite(ledPin, HIGH);  // LED OFF
    delay(100);                 // 0.1초
    digitalWrite(ledPin, LOW);   // LED ON
    delay(100);                 // 0.1초
  }

  // 마지막에 LED 꺼진 상태로 멈춤
  digitalWrite(ledPin, HIGH);  // OFF
  while (1) {}  // 무한루프
}
