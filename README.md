# 🏗️ Safety Construction Site  
> **센서 기반 실시간 안전 관리 IoT 시스템**  
> 블루투스 → 라즈베리파이 → ThingsBoard 로 데이터 송수신 및 시각화

---

## 📖 프로젝트 개요
이 프로젝트는 **공사 현장의 작업자 안전 상태를 실시간으로 모니터링하기 위한 IoT 시스템**입니다.  
아두이노 기반 센서 모듈이 현장의 데이터를 수집하고, 라즈베리파이를 통해 MQTT로 ThingsBoard에 전송합니다.

---

## ⚙️ 시스템 구성

### 🧩 하드웨어
- **Arduino Mega / Uno** : 센서 데이터 수집  
- **센서 구성 예시**
  - MQ135 (유해가스)
  - MPU6050 (자이로/가속도)
  - TTP223 (터치)
  - NEO-6M (GPS)
- **Bluetooth (HC-06)** : 아두이노 → 라즈베리파이 통신  
- **Raspberry Pi 3B+** : MQTT 클라이언트, ThingsBoard 업로드 담당  

### 💻 소프트웨어
- **Python3**  
  - 블루투스 통신(JSON 파싱)
  - 로컬 로그 저장(NDJSON)
  - ThingsBoard MQTT 전송
- **ThingsBoard**
  - 실시간 데이터 대시보드 및 알림 설정

---

## 🗂️ 파일 구조 예시
Safety-construction-site/
├─ FINAL.ino # 아두이노 센서 데이터 
├─ sensor_data.json # 수집된 로컬 로그 (NDJSON)
└─ Rasberry.py # 블루투스 → MQTT 중계 코드


---

## 🚀 실행 방법

### 1️⃣ 의존성 설치
```bash
sudo apt install python3-pip
pip install pybluez paho-mqtt
```
### 2️⃣ 블루투스 MAC 주소 확인
hcitool scan


→ 예: 98:DA:60:0C:F0:24

### 3️⃣ 실행
python3 Rasberry.py

성공 시:
[INFO] Bluetooth connected
{"gas":132,"gyro":0.02,"timestamp":"2025-10-27 12:00:31"}

📊 ThingsBoard 설정

ThingsBoard 대시보드 접속

디바이스 생성 후 Access Token 복사

main.py의 TB_TOKEN에 붙여넣기

대시보드 위젯에서 실시간 데이터 확인 가능

🧱 주요 기능

✅ 센서 데이터 JSON 실시간 파싱
✅ NDJSON 포맷으로 로컬 백업
✅ MQTT 전송 및 자동 재연결
✅ 블루투스 통신 예외 처리(Timeout, Busy 등)
✅ 확장 가능한 구조 (센서 추가 용이)

📜 라이선스

이 프로젝트는 개인 포트폴리오 및 학습 목적용으로 제작되었습니다.
상업적 이용 시 사전 허락이 필요합니다.

🧑‍💻 제작자

라피 (LaPPP)
컴퓨터전자공학 전공 / IoT & 임베디드 시스템 개발자 지망





-----------------------------------------------------------------
# 🏗️ Safety Construction Site  
> **Sensor-based Real-Time Safety Management IoT System**  
> Data flow: **Bluetooth → Raspberry Pi → ThingsBoard** (for data transmission and visualization)

---

## 📖 Overview
This project is an **IoT-based system designed to monitor the safety status of construction site workers in real time.**  
Sensor modules built on Arduino collect field data, which are then transmitted to a Raspberry Pi via Bluetooth and sent to ThingsBoard through MQTT.

---

## ⚙️ System Architecture

### 🧩 Hardware
- **Arduino Mega / Uno** – Collects sensor data  
- **Sensor Modules**
  - MQ135 (Toxic Gas Detection)
  - MPU6050 (Gyroscope / Accelerometer)
  - TTP223 (Touch Sensor)
  - NEO-6M (GPS Module)
- **Bluetooth (HC-06)** – Communication between Arduino and Raspberry Pi  
- **Raspberry Pi 3B+** – Acts as MQTT client, sends data to ThingsBoard  

### 💻 Software
- **Python 3**
  - Handles Bluetooth communication and JSON parsing  
  - Saves local logs in NDJSON format  
  - Publishes telemetry data to ThingsBoard via MQTT  
- **ThingsBoard**
  - Provides real-time dashboards and alert management  

---

## 🗂️ Project Structure
Safety-construction-site/
├─ FINAL.ino # Arduino sensor data collection code
├─ sensor_data.json # Local log file (NDJSON format)
└─ Rasberry.py # Bluetooth → MQTT bridge script

yaml
코드 복사

---

## 🚀 How to Run

### 1️⃣ Install Dependencies
```bash
sudo apt install python3-pip
pip install pybluez paho-mqtt
2️⃣ Check Bluetooth MAC Address
bash
코드 복사
hcitool scan
Example: 98:DA:60:0C:F0:24

3️⃣ Run the Script
bash
코드 복사
python3 Rasberry.py
Expected output:

arduino
코드 복사
[INFO] Bluetooth connected
{"gas":132,"gyro":0.02,"timestamp":"2025-10-27 12:00:31"}
📊 ThingsBoard Configuration
Open the ThingsBoard dashboard.

Create a new device and copy its Access Token.

Paste the token into TB_TOKEN inside Rasberry.py.

Monitor real-time telemetry on the dashboard widgets.
```
🧱 Key Features
✅ Real-time sensor data parsing (JSON)
✅ Local backup using NDJSON format
✅ MQTT transmission with auto-reconnect
✅ Robust Bluetooth error handling (Timeout / Busy)
✅ Easily expandable structure for new sensors

📜 License
This project was developed for personal portfolio and educational purposes.
Commercial use requires prior permission.

🧑‍💻 Developer
LaPPP (라피)
Department of Computer & Electronic Engineering
Aspired Embedded & IoT System Developer
