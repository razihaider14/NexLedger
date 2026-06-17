#pragma once

#define PIN_RC522_SS 5
#define PIN_RC522_RST 4

#define LCD_ADDRESS 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

#define PIN_LED_GREEN 26
#define PIN_LED_RED 25
#define PIN_BUZZER 27

#define TOPIC_SCAN "ledger/scan"
#define TOPIC_STATUS "ledger/status"
#define TOPIC_RESULT "ledger/result"
#define TOPIC_ENROLL_START "ledger/cmd/enroll"
#define TOPIC_ENROLL_SCANNED "ledger/cmd/enroll/scanned"

#define MQTT_PORT 8883
#define HEARTBEAT_INTERVAL 30000
#define SCAN_COOLDOWN 2000
#define FEEDBACK_DURATION 1500

static const char CA_CERT[] = R"EOF(
-----BEGIN CERTIFICATE-----
your_ca_cert_here
-----END CERTIFICATE-----
)EOF";
