// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12   // sonar sensor TRIGGER
#define PIN_ECHO 13   // sonar sensor ECHO

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25       // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100.0   // minimum distance (mm)
#define _DIST_MAX 300.0   // maximum distance (mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL)     // coefficent to convert duration to distance

unsigned long last_sampling_time;   // unit: msec
float distance = _DIST_MAX;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);  // sonar TRIGGER
  pinMode(PIN_ECHO, INPUT);   // sonar ECHO
  digitalWrite(PIN_TRIG, LOW);  

  Serial.begin(57600);
}

void loop() { 
  if (millis() < (last_sampling_time + INTERVAL))
    return;

  distance = USS_measure(PIN_TRIG, PIN_ECHO); // read distance
  int brightness;

  if ((distance == 0.0) || (distance > _DIST_MAX)) {
      brightness = 255;       // LED OFF (active low)
  } else if (distance < _DIST_MIN) {
      brightness = 255;       // LED OFF
  } else {
      // 거리 비례해서 밝기 조절 (200mm에서 최댓값, 100mm/300mm에서 최소값)
      if (distance <= 200.0) {
        brightness = map(distance, 100, 200, 255, 0);  // 100→꺼짐, 200→최대밝기
      } else {
        brightness = map(distance, 200, 300, 0, 255);  // 200→최대밝기, 300→꺼짐
      }
  }

  analogWrite(PIN_LED, brightness);

  // ---- Serial Plotter 출력 ----
  Serial.print("distance:"); 
  Serial.print(distance);
  Serial.print("\tbrightness:");
  Serial.println(brightness);

  last_sampling_time += INTERVAL;
}

// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm
}
