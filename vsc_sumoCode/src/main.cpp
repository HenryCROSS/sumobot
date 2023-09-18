#include <Arduino.h>
// #include <OLED_utils.hpp>
#include <Configs.h>
#include <Functional_interface.hpp>
#include <Tools.hpp>
#include <Vehicle_actions.hpp>
#include <Vehicle_utils.hpp>
#include <string.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 1);

void setup()
{
    Serial.begin(9600);
    // put your setup code here, to run once:
    pinMode(TRIGGER_PIN_L, OUTPUT);
    pinMode(ECHO_PIN_L, INPUT);
    pinMode(TRIGGER_PIN_R, OUTPUT);
    pinMode(ECHO_PIN_R, INPUT);
    pinMode(LEFT_WHEEL_FORWARD, OUTPUT);
    pinMode(LEFT_WHEEL_BACKWARD, OUTPUT);
    pinMode(RIGHT_WHEEL_FORWARD, OUTPUT);
    pinMode(RIGHT_WHEEL_BACKWARD, OUTPUT);
    pinMode(LEFT_MOTOR, OUTPUT);
    pinMode(RIGHT_MOTOR, OUTPUT);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    { // Address 0x3D for 128x64
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    }
    delay(2000);

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    // Display static text
    display.println("Hello, world!");
    display.display();
    delay(1000);
}

struct Test
{
    static void oled_display(const char *msg)
    {
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0, 10);
        // Display static text
        display.println(msg);
        display.display();
    }
    static void motor_forward(bool left_wheel, bool right_wheel, int speed)
    {
        if (left_wheel && right_wheel)
        {
            car_go_forward(speed);
        }
        else if (left_wheel)
        {
            car_turn_right(speed);
        }
        else if (right_wheel)
        {
            car_turn_left(speed);
        }
    }
    static void motor_backward(bool left_wheel, bool right_wheel, int speed)
    {
        if (left_wheel && right_wheel)
        {
            car_go_backward(speed);
        }
        else if (left_wheel)
        {
            car_turn_left_by_speed(speed, 0);
        }
        else if (right_wheel)
        {
            car_turn_right_by_speed(0, speed);
        }
    }
    static void ultrasonic_test(bool left, bool right, int range, int speed)
    {
        auto info = obj_detection_info();
        if (left && info.left_sensor.hasValue() && right && info.right_sensor.hasValue())
        {
            car_go_backward(speed);
        }
        else if (left && info.left_sensor.hasValue())
        {
            car_go_forward(speed);
        }
        else if (right && info.right_sensor.hasValue())
        {
            car_go_forward(speed);
        }
    }
    static void qtr_test(bool frontL, bool frontR, bool back, bool noEdge, int speed)
    {
        auto info = determine_edge(QTR_SENSOR_FL, QTR_SENSOR_FR, QTR_SENSOR_B);
        if (info.hasValue())
        {
            switch (info.getValue())
            {
            case Edge_Signal::FRONT_LEFT:
                if (frontL)
                {
                    car_go_forward(speed);
                }
                break;
            case Edge_Signal::FRONT_RIGHT:
                if (frontR)
                {
                    car_go_forward(speed);
                }
                break;
            case Edge_Signal::BACK:
                if (back)
                {
                    car_go_forward(speed);
                }
                break;
            default:
                // never exec
                break;
            }
        }
        else
        {
            if (noEdge)
            {
                car_go_backward(speed);
            }
        }
    }
    static void trace_mode()
    {
        auto speed = 130;
        auto range = 30.0;
        auto tolerance = 1;
        auto info = obj_detection_info();
        auto delay_ms = 10;

        Serial.println(String(">>L: ") + info.left_sensor.getValue());
        Serial.println(String(">>R: ") + info.right_sensor.getValue());

        if (is_obj_in_distance(info, range))
        {
            if (is_adjusting_needed(info, range, tolerance))
            {
                car_adjust_attack_direction(info, tolerance, speed);
                Test::oled_display("TURNING");
            }
            else
            {
                car_go_forward(speed);
                Test::oled_display("FORWARD");
            }
        }
        else
        {
            Test::oled_display("NOT MOVE");
            Serial.println("NOT MOVE");
        }

        delay(delay_ms);
    }

    static void normal_mode()
    {
    }
};

void loop()
{
    Serial.println("===============");
    Test::trace_mode();
    delay(500);
}