#include "mqtt_handler.h"
#include "config.h"
#include "feedback.h"
#include "display.h"
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Preferences.h>
#include <ArduinoJson.h>

static WiFiClientSecure secureClient;
static PubSubClient mqttClient(secureClient);
static Preferences prefs;

static String mqttBroker = "";
static String mqttUser = "";
static String mqttPassword = "";

static unsigned long lastHeartbeat = 0;
static unsigned long lastReconnectAt = 0;
#define RECONNECT_INTERVAL 5000

void rfidClearLastUID();
extern bool enrollMode;

static void onMessage(char* topic, byte* payload, unsigned int length) {
    String t = String(topic);
    String msg = "";
    for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];

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
            const char* reason = doc["reason"] | "Try again";
            displayMessage("Declined", reason);
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

static void loadCredentials() {
    prefs.begin("nexledger", true);
    mqttBroker = prefs.getString("mqtt_ip", "");
    mqttUser = prefs.getString("mqtt_user", "");
    mqttPassword = prefs.getString("mqtt_pass", "");
    prefs.end();
}

static void connectMQTT() {
    if (mqttClient.connected()) return;

    unsigned long now = millis();
    if (now - lastReconnectAt < RECONNECT_INTERVAL) return;
    lastReconnectAt = now;

    displayMessage("MQTT", "Connecting...");

    secureClient.setCACert(CA_CERT);
    mqttClient.setServer(mqttBroker.c_str(), MQTT_PORT);
    mqttClient.setCallback(onMessage);

    if (mqttClient.connect("NexLedger", mqttUser.c_str(), mqttPassword.c_str())) {
        mqttClient.subscribe(TOPIC_RESULT);
        mqttClient.subscribe(TOPIC_ENROLL_START);
        displayMessage("NexLedger", "Ready");
    } else {
        displayMessage("MQTT Failed", "Retrying...");
    }
}

void mqttInit() {
    loadCredentials();
    connectMQTT();
}

void mqttUpdate() {
    if (!mqttClient.connected()) {
        connectMQTT();
        return;
    }
    mqttClient.loop();

    unsigned long now = millis();
    if (now - lastHeartbeat >= HEARTBEAT_INTERVAL) {
        lastHeartbeat = now;
        mqttClient.publish(TOPIC_STATUS, "online");
    }
}

void mqttPublishScan(const String& uid) {
    StaticJsonDocument<64> doc;
    doc["uid"] = uid;
    char buf[64];
    serializeJson(doc, buf);
    mqttClient.publish(TOPIC_SCAN, buf);
}