#include <Wire.h>
#include <TinyGPS++.h>
#include <MQ135.h>

//------------BUZZER--------
const int buz = 12;

// ------------LED -------
const int l8 = 8;
const int l9 = 9;
const int l10 = 10;
const int l11 = 11;

// ---------------- MQ135 ----------------
const int mq135Pin = A0;
MQ135 mq135_sensor(mq135Pin);
float ppm_value = 0.0;

// ---------------- MPU6050 ----------------
#define MPU 0x68
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

// ---------------- GPS ----------------
TinyGPSPlus gps;

// ---------------- TTP223 ----------------
const int touchPin = 2;
int touchState = 0;

// ---------------- 주기 관리 ----------------
unsigned long lastSensorUpdate = 0;
const unsigned long sensorInterval = 1000;

void setup() {
  Serial.begin(9600);

  pinMode(l8, OUTPUT);
  pinMode(l9, OUTPUT);
  pinMode(l10, OUTPUT);
  pinMode(l11, OUTPUT);
  pinMode(buz, OUTPUT);
  pinMode(mq135Pin, INPUT);
  pinMode(touchPin, INPUT);

  Serial1.begin(9600);   // Bluetooth
  Serial2.begin(115200);   // GPS (NEO-6M 기본 9600bps)

  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  Serial.println("=== All Sensor + GPS + Bluetooth + MQ135(ppm) Test Start ===");
}

void loop() {
  // GPS 처리
  while (Serial2.available() > 0) {
    gps.encode(Serial2.read());
  }

  // 1초마다 센서 업데이트
  if (millis() - lastSensorUpdate >= sensorInterval) {
    lastSensorUpdate = millis();

    // ----- MQ135 (PPM 계산) -----
    ppm_value = mq135_sensor.getPPM();  // 라이브러리 함수로 ppm값 가져오기

    String airQuality = "Good";
    if (ppm_value > 30000)
      airQuality = "Very Bad";
    else if (ppm_value > 5000)
      airQuality = "Bad";
    else if (ppm_value > 1000)
      airQuality = "Normal";

    // ----- MPU6050 -----
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 14, true);

    AcX = Wire.read() << 8 | Wire.read();
    AcY = Wire.read() << 8 | Wire.read();
    AcZ = Wire.read() << 8 | Wire.read();
    Tmp = Wire.read() << 8 | Wire.read();
    GyX = Wire.read() << 8 | Wire.read();
    GyY = Wire.read() << 8 | Wire.read();
    GyZ = Wire.read() << 8 | Wire.read();

    float temperature = Tmp / 340.0 + 36.53;

    // ----- Touch Sensor -----
    touchState = digitalRead(touchPin);

    // ----- GPS -----
    double latitude = gps.location.isValid() ? gps.location.lat() : 0.0;
    double longitude = gps.location.isValid() ? gps.location.lng() : 0.0;
    int sats = gps.satellites.value();
    float hdop = gps.hdop.hdop();

    // ----- JSON 데이터 구성 -----
    String json = "{";
    json += "\"ppm\":" + String(ppm_value, 2);
    json += ",\"gasLevel\":\"" + airQuality + "\"";
    json += ",\"accelX\":" + String(AcX);
    json += ",\"accelY\":" + String(AcY);
    json += ",\"accelZ\":" + String(AcZ);
    json += ",\"gyroX\":" + String(GyX);
    json += ",\"gyroY\":" + String(GyY);
    json += ",\"gyroZ\":" + String(GyZ);
    json += ",\"temperature\":" + String(temperature, 2);
    json += ",\"latitude\":" + String(latitude, 6);
    json += ",\"longitude\":" + String(longitude, 6);
    json += ",\"satellites\":" + String(sats);
    json += ",\"hdop\":" + String(hdop, 2);
    json += ",\"touch\":" + String(touchState);
    json += "}";

    // ----- 출력 -----
    Serial.println(json);
    Serial1.println(json);

    // ----- LED / BUZZER 제어 -----
    noTone(buz);
    if (airQuality == "Good") {
      digitalWrite(l8, HIGH);
      digitalWrite(l9, LOW);
      digitalWrite(l10, LOW);
      digitalWrite(l11, LOW);
    }
    else if (airQuality == "Normal") {
      digitalWrite(l8, HIGH);
      digitalWrite(l9, HIGH);
      digitalWrite(l10, LOW);
      digitalWrite(l11, LOW);
    }
    else if (airQuality == "Bad") {
      digitalWrite(l8, HIGH);
      digitalWrite(l9, HIGH);
      digitalWrite(l10, HIGH);
      digitalWrite(l11, LOW);
      tone(buz, 349);
    }
    else if (airQuality == "Very Bad") {
      digitalWrite(l8, HIGH);
      digitalWrite(l9, HIGH);
      digitalWrite(l10, HIGH);
      digitalWrite(l11, HIGH);
      tone(buz, 494);
    }
  }
}
