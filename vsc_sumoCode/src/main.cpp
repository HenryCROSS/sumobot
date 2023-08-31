#include <Arduino.h>
#include <Configs.h>
#include <OLED_utils.hpp>
#include <Tools.hpp>
#include <Vehicle_actions.hpp>
#include <Vehicle_utils.hpp>
#include <Functional_interface.hpp>

void setup()
{
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
    // pinMode(QTR_SENSOR_FL, INPUT);
    // pinMode(QTR_SENSOR_FR, INPUT);
    // pinMode(QTR_SENSOR_B, INPUT);

    initScreen(SSD1306_SWITCHCAPVCC, 0x3C);

    if (!global_display.hasValue())
    { // Address 0x3D for 128x64
        Serial.println(F("SSD1306 allocation failed"));
        delay(5000);
    }
    else
    {
        screen_count_down_3sec();
    }

    randomSeed(analogRead(0));
    Serial.begin(9600);
}

struct Test
{
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
        auto info = obj_detected_info(range);
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
        auto range = 20.0;
        auto tolerance = 1.2;
        auto info = obj_detected_info(range);
        auto delay_ms = 10;

        if (is_obj_in_distance(info, range))
        {
            if (is_adjusting_needed(info, range, tolerance))
            {
                car_adjust_attack_direction(info, tolerance, speed);
            }
            else
            {
                car_go_forward(speed);
            }
        }

        delay(delay_ms);
    }
};

void loop()
{
    Serial.println("working");
    Test::trace_mode();
}