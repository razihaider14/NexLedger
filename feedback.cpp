#include "feedback.h"
#include "config.h"
#include <Arduino.h>

enum FeedbackState {
    FB_IDLE,
    FB_SUCCESS,
    FB_FAILURE,
    FB_ENROLL_MODE,
    FB_ENROLL_SUCCESS
};

static FeedbackState currentState = FB_IDLE;
static unsigned long stateStart = 0;
static int blinkStep = 0;
static unsigned long lastBlinkTime = 0;

void feedbackInit() {
    pinMode(PIN_LED_GREEN, OUTPUT);
    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_BUZZER, OUTPUT);
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_BUZZER, LOW);
}

void feedbackSuccess() {
    currentState = FB_SUCCESS;
    stateStart = millis();
    digitalWrite(PIN_LED_GREEN, HIGH);
    digitalWrite(PIN_BUZZER, HIGH);
}

void feedbackFailure() {
    currentState = FB_FAILURE;
    stateStart = millis();
    digitalWrite(PIN_LED_RED, HIGH);
    digitalWrite(PIN_BUZZER, HIGH);
}

void feedbackEnrollMode() {
    currentState = FB_ENROLL_MODE;
    stateStart = millis();
    lastBlinkTime = millis();
    blinkStep = 0;
    digitalWrite(PIN_LED_GREEN, HIGH);
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_BUZZER, HIGH);
}

void feedbackEnrollSuccess() {
    currentState = FB_ENROLL_SUCCESS;
    stateStart = millis();
    lastBlinkTime = millis();
    blinkStep = 0;
    digitalWrite(PIN_LED_GREEN, HIGH);
}

void feedbackUpdate() {
    unsigned long now = millis();

    switch (currentState) {

        case FB_SUCCESS:
            if (now - stateStart >= FEEDBACK_DURATION) {
                digitalWrite(PIN_LED_GREEN, LOW);
                digitalWrite(PIN_BUZZER, LOW);
                currentState = FB_IDLE;
            }
            break;

        case FB_FAILURE:
            if (now - stateStart >= FEEDBACK_DURATION) {
                digitalWrite(PIN_LED_RED, LOW);
                digitalWrite(PIN_BUZZER, LOW);
                currentState = FB_IDLE;
            }
            break;

        case FB_ENROLL_MODE:
            if (blinkStep < 6 && now - lastBlinkTime >= 150) {
                lastBlinkTime = now;
                blinkStep++;
                if (blinkStep % 2 == 0) {
                    digitalWrite(PIN_LED_GREEN, HIGH);
                    digitalWrite(PIN_LED_RED, LOW);
                } else {
                    digitalWrite(PIN_LED_GREEN, LOW);
                    digitalWrite(PIN_LED_RED, HIGH);
                }
                if (blinkStep == 1) digitalWrite(PIN_BUZZER, HIGH);
                if (blinkStep == 2) digitalWrite(PIN_BUZZER, LOW);
            }
            if (blinkStep >= 6) {
                digitalWrite(PIN_LED_GREEN, LOW);
                digitalWrite(PIN_LED_RED, LOW);
                digitalWrite(PIN_BUZZER, LOW);
                currentState = FB_IDLE;
            }
            break;

        case FB_ENROLL_SUCCESS:
            if (blinkStep < 4 && now - lastBlinkTime >= 200) {
                lastBlinkTime = now;
                blinkStep++;
                digitalWrite(PIN_LED_GREEN, blinkStep % 2 == 0 ? LOW : HIGH);
            }
            if (blinkStep >= 4) {
                digitalWrite(PIN_LED_GREEN, LOW);
                currentState = FB_IDLE;
            }
            break;

        case FB_IDLE:
        default:
            break;
    }
}