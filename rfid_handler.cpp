#include "rfid_handler.h"
#include "config.h"
#include "feedback.h"
#include "display.h"
#include "mqtt_handler.h"
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

static MFRC522 mfrc522(PIN_RC522_SS, PIN_RC522_RST);
static unsigned long lastScanTime = 0;
static String lastUID = "";

bool enrollMode = false;

static String getUID(MFRC522::Uid uid) {
    String result = "";
    for (byte i = 0; i < uid.size; i++) {
        if (uid.uidByte[i] < 0x10) result += "0";
        result += String(uid.uidByte[i], HEX);
    }
    result.toUpperCase();
    return result;
}

void rfidInit() {
    SPI.begin();
    mfrc522.PCD_Init();
}

void rfidUpdate() {
    if (!mfrc522.PICC_IsNewCardPresent()) return;
    if (!mfrc522,PICC_ReadCardSerial()) return;

    unsigned long now = millis();
    if (now - lastScanTime < SCAN_COOLDOWN) return;
    lastScanTime = now;

    String uid = getUID(mfrc522.uid);
    mfrc522.PICC_HaltA();

    if (enrollMode) {
        mqttPublishEnrollScanned(uid);
        displayMessage("Card Scanned", "Enrolling...");
        feedbackEnrollSuccess();
        enrollMode = false;
        return;
    }

    if (uid == lastUID) return;

    lastUID = uid;
    mqttPublishScan(uid);
    displayMessage("Card Scanned", "Please wait...");
}

void rfidClearLastUID() {
    lastUID = "";
}