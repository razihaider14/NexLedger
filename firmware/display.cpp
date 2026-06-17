#include "display.h"
#include "config.h"
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

static LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);

void displayInit() {
    lcd.init();
    lcd.backlight();
    displayMessage("NexLedger", "Initializing...");
}

void displayClear() {
    lcd.clear();
}

void displayMessage(const char* line1, const char* line 2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1);
    if (line2 && line2[0] != '\0') {
        lcd.setCursor(0, 1);
        lcd.print(line2);
    }
}