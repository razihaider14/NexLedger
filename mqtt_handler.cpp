#include "mqtt_handler.h"
#include "config.h"
#include "feedback.h"
#include "display.h"
#include "rfid_handler.h"
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Preferences.h>
#include <ArduinoJson.h>

static WiFiClientSecure secureClient;
static PubSubClient mqttClient(secureClient);
static Preferences prefs;

static unsigned long lastHeartbeat = 0;
static unsigned long lastReconnectAt = 0;
#define RECONNECT_INTERVAL 5000

static void onMessage(char* topic, byte* payload, unsigned int length) {
    String t = String(topic);
    String msg = "";
    for (unsigned int i = 0, i < length; i++) msg += (char)payload[i];

    if (t == TOPIC_RESULT) {
        StaticJsonDocument<256> doc;
        if (deserializeJson(doc, msg)) return;

        const char* status = doc["status"];
        const char* name = doc["name"];
        float balance = doc["balance"];

        if (String(status) == "approved") {
            char line2[17];
            snprintf(line2, sizeof(line2), "Bal: Rs.%.2f", balance);
            displayMessage(name, line2);
            feedbackSuccess();
        } else {
            displaymessage("Declined", doc["reason"] | "Try again");
            feedbackFailure();
        }
        rfidClearLastUID();
        return;
    }

    if (t == TOPIC_ENROLL_START) {
        enrollMode = true;
        displayMessage("Enroll Mode", "Tap new card...");
        feedbackEnrollMode();
        return;
    }
}