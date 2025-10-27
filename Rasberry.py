#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import bluetooth
import json
import time
import logging
import paho.mqtt.client as mqtt
from json import JSONDecoder

# ---- Bluetooth target ----    // HC-06 시리얼 통신 모듈 연결 정보
TARGET_NAME = ""
TARGET_MAC  = ""  # RFCOMM ch.1 (HC-06 기본)

# ---- Local save (NDJSON) ----
SAVE_FILE   = "sensor_data.json"

# ---- ThingsBoard MQTT ----    // 자신의 ThingsBoard 서버 정보로 변경
TB_HOST  = ""
TB_PORT  = 
TB_TOKEN = ""
TB_TOPIC = ""

# ---- Logging ----
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s %(levelname)s %(message)s",
)

def make_mqtt():
    client = mqtt.Client()
    client.username_pw_set(TB_TOKEN)
    client.connect(TB_HOST, TB_PORT, 60)
    client.loop_start()
    return client

mqttc = None
def tb_publish(payload: dict):
    """Publish telemetry to ThingsBoard (qos=1)."""
    global mqttc
    try:
        if mqttc is None:
            mqttc = make_mqtt()
        mqttc.publish(TB_TOPIC, json.dumps(payload), qos=1)
    except Exception as e:
        logging.warning("MQTT publish failed, will re-init on next call: %s", e)
        try:
            if mqttc is not None:
                mqttc.loop_stop()
        except Exception:
            pass
        mqttc = None

logging.info("Bluetooth connect: %s (%s)", TARGET_NAME, TARGET_MAC)
try:
    sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
    sock.connect((TARGET_MAC, 1))
    sock.settimeout(2.0)
    logging.info("Bluetooth connected")
except Exception as e:
    logging.error("Bluetooth connect failed: %s", e)
    raise SystemExit(1)

decoder = JSONDecoder()
buffer  = ""

def flush_jsons_from_buffer():
    """
    Parse as many JSON objects as possible from the global buffer.
    Consumed bytes are trimmed from the front of the buffer.
    """
    global buffer
    results = []
    idx = 0
    n = len(buffer)

    first_brace = buffer.find('{', idx)
    if first_brace > 0:
        buffer = buffer[first_brace:]
        n = len(buffer)
        idx = 0

    while idx < n:
        brace_pos = buffer.find('{', idx)
        if brace_pos == -1:
            if idx > 0:
                buffer = buffer[idx:]
            return results
        if brace_pos > idx:
            idx = brace_pos
        try:
            obj, end = decoder.raw_decode(buffer[idx:])
            results.append(obj)
            idx += end
        except json.JSONDecodeError:
            break

    buffer = buffer[idx:]
    return results

try:
    while True:
        try:
            chunk = sock.recv(1024)
            if not chunk:
                continue

            text = chunk.decode("utf-8", errors="replace")
            buffer += text

            for item in flush_jsons_from_buffer():
                item["timestamp"] = time.strftime("%Y-%m-%d %H:%M:%S")

                # stdout: 단문 JSON 한 줄
                print(json.dumps(item, separators=(",", ":")))

                # 1) local NDJSON
                with open(SAVE_FILE, "a", encoding="utf-8") as f:
                    f.write(json.dumps(item, ensure_ascii=False) + "\n")

                # 2) ThingsBoard publish
                tb_publish(item)

        except bluetooth.btcommon.BluetoothError:
            # timeout 등 일시 오류는 루프 지속
            continue

except KeyboardInterrupt:
    logging.info("Interrupted by user")
finally:
    try:
        sock.close()
    except Exception:
        pass
    try:
        if mqttc is not None:
            mqttc.loop_stop()
    except Exception:
        pass
    logging.info("Bluetooth socket closed; MQTT loop stopped")
