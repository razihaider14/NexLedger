#pragma once

void mqttInit();
void mqttUpdate();
void mqttPublishScan(const String& uid);
void mqttPublishEnrollScanned(const String& uid);