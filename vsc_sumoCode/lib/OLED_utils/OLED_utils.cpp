#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "OLED_utils.hpp"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool initScreen(uint8_t switchvcc, uint8_t i2caddr,
                bool reset, bool periphBegin)
{

    // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
    return display.begin(switchvcc, i2caddr, reset, periphBegin);
}

void screen_count_down_4sec()
{
    delay(1000);
    screen_display_after_clear("4", 0, 0);
    delay(1000);
    screen_display_after_clear("3", 0, 0);
    delay(1000);
    screen_display_after_clear("2", 0, 0);
    delay(1000);
    screen_display_after_clear("1", 0, 0);
    delay(1000);
}

void screen_display_after_clear(const char *msg, int16_t x, int16_t y, uint8_t size)
{

    display.clearDisplay();
    display.setTextSize(size);
    display.setTextColor(WHITE);
    display.setCursor(x, y);
    display.print(msg);
    display.display();
}

void screen_display_without_clear(const char *msg, int16_t x, int16_t y, uint8_t size)
{

    display.setTextSize(size);
    display.setTextColor(WHITE);
    display.setCursor(x, y);
    display.print(msg);
    display.display();
}

void screen_display_data(Adafruit_SSD1306 display, Display_type type, const char *data)
{
    switch (type)
    {
    case Display_type::LEFT_WHEEL:

        break;

    default:
        break;
    }

    display.display();
}