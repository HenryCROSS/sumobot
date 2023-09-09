#ifndef OLED_UTILS_HPP
#define OLED_UTILS_HPP

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#include <Tools.hpp>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

enum class Display_type
{
    LEFT_WHEEL,
    RIGHT_WHEEL,
    LEFT_ULTRASONIC,
    RIGHT_ULTRASONIC,
    FL_QTR,
    FR_QTR,
    B_QTR,
};

extern Adafruit_SSD1306 display;

bool initScreen(uint8_t switchvcc = SSD1306_SWITCHCAPVCC, uint8_t i2caddr = 0,
                bool reset = true, bool periphBegin = true);

void screen_count_down_4sec();

void screen_display_after_clear(const char *msg, int16_t x, int16_t y, uint8_t size = 1u);
void screen_display_without_clear(const char *msg, int16_t x, int16_t y, uint8_t size = 1u);
void screen_display_data(Adafruit_SSD1306 display, Display_type type, const char *data);

#endif // !OLED_UTILS_HPP
