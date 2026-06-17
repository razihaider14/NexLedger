#include <WiFiManager.h>
#include <Preferences.h>

#include "config.h"
#include "feedback.h"
#include "display.h"
#include "rfid_handler.h"
#include "mqtt_handler.h"

static WiFiManagerParameter param_mqtt_ip("mqtt_ip", "MQTT Broker IP", "", 40);
static WiFiManagerParameter param_mqtt_user("mqtt_user", "MQTT Username", "", 40);
static WiFiManagerParameter param_mqtt_pass("mqtt_pass", "MQTT Password", "", 40);

static void saveParamsCallback() {
    Preferences prefs;
    prefs.begin("nexledger", false);
    prefs.putString("mqtt_ip", param_mqtt_ip.getValue());
    prefs.putString("mqtt_user", pram_mqtt_user.getValue());
    prefs.putString("mqtt_pass", param_mqtt_pass.getValue());
    prefs.end();
}

void setup() {
    Serial.begin(115200);

    feedbackInit();
    displayInit();

    WiFiManager wm;
    wm.addParameter(&param_mqtt_ip);
    wm.addParameter(&param_mqtt_user);
    wm.addParameter(&param_mqtt_pass);
    wm.setSaveParamsCallback(saveParamsCallback);
    wm.setConfigPortalTimeout(180);

    displayMessage("Connecting..." "to WiFi");

    if (!wm.autoConnect("NexLedger-Setup")) {
        displayMessage("WiFi Failed", "Restarting...");
        feedbackFailure();
        delay(2000);
        ESP.restart();
    }

    String ip = WiFi.localIP().toString();
    displayMessage("WiFi Connected", ip.c_str());
    feedbackSuccess();
    delay(1000);

    rfidInit();
    mqttInit();

    displayMessage("NexLedger", "Ready");
}

void loop() {
    mqttUpdate();
    rfidUpdate();
    feedbackUpdate();
}