// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12   // sonar sensor TRIGGER
#define PIN_ECHO 13   // sonar sensor ECHO

// configurable parameters (slides applied)
#define SND_VEL 346.0       // m/s @ 24°C
#define INTERVAL 25         // sampling interval (msec)
#define PULSE_DURATION 10   // TRIG pulse (usec)
#define _DIST_MIN 100.0     // mm : 꺼짐
#define _DIST_PEAK 200.0    // mm : 최대 밝기
#define _DIST_MAX 300.0     // mm : 꺼짐

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // echo timeout (usec) ≈ 12.5 ms
#define SCALE (0.001 * 0.5 * SND_VEL)     // duration(us) -> distance(mm)

unsigned long last_sampling_time = 0;     // unit: msec

// ---------- utils ----------
static inline int clamp255(int v) { return v < 0 ? 0 : (v > 255 ? 255 : v); }

// 거리(mm) -> PWM duty(0~255) (active-low 기준: 0=가장 밝음, 255=꺼짐)
int duty_from_distance(float mm) {
  if (mm <= 0)          return 255;   // 측정 실패
  if (mm <= _DIST_MIN)  return 255;   // 100mm 이하
  if (mm >= _DIST_MAX)  return 255;   // 300mm 이상
  if (mm == _DIST_PEAK) return 0;     // 200mm

  if (mm < _DIST_PEAK) {
    // 100 → 200 mm : 255 → 0 (밝아짐)
    float r = (mm - _DIST_MIN) / (_DIST_PEAK - _DIST_MIN); // 0~1
    return clamp255((int)(255.0f * (1.0f - r) + 0.5f));
  } else {
    // 200 → 300 mm : 0 → 255 (어두워짐)
    float r = (mm - _DIST_PEAK) / (_DIST_MAX - _DIST_PEAK); // 0~1
    return clamp255((int)(255.0f * r + 0.5f));
  }
}

// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);

  unsigned long dur = pulseIn(ECHO, HIGH, TIMEOUT); // us
  if (dur == 0) return 0.0;     // 타임아웃 → 실패
  return dur * SCALE;           // mm
}

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  Serial.begin(57600);
}

void loop() {
  // 정확한 25 ms 주기
  if (millis() - last_sampling_time < INTERVAL) return;
  last_sampling_time = millis();

  float distance = USS_measure(PIN_TRIG, PIN_ECHO); // mm
  int duty = duty_from_distance(distance);          // 0~255 (active-low)

  analogWrite(PIN_LED, 255 - duty); // active-low: 0=밝음, 255=꺼짐
  // active-high 배선이면 아래처럼 바꿔:
  // analogWrite(PIN_LED, 255 - duty);

  // 시리얼 플로터용 (두 곡선)
  Serial.print("distance:");
  Serial.print(distance, 1);
  Serial.print("\tduty:");
  Serial.println(duty);
}
