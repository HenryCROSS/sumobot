#include <Arduino.h>
// #include <OLED_utils.hpp>
#include <Configs.h>
#include <MK2System.hpp>
#include <Vehicle_actions.hpp>
#include <Vehicle_utils.hpp>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 1);

void task_searching(MK2System::VehState &state)
{
    state.ultra_info = obj_detection_info();
}

void task_qtr(MK2System::VehState &state)
{
    state.edge_info = determine_edge(QTR_SENSOR_FL, QTR_SENSOR_FR, QTR_SENSOR_B);
}

void task_backward(MK2System::VehState &state)
{
    if (state.stage != MK2System::Stage::BACKWARD)
        return;

    if (state.edge_info.hasValue())
    {
        state.stage = MK2System::Stage::BATTLE;
    }
    else
    {
        car_turn_right_by_speed(130, 130);
        delay(TIMESLICE);
        car_go_backward(160);
        delay(TIMESLICE * 3);
    }
}

void task_normal_attack(MK2System::VehState &state)
{
    if (state.stage != MK2System::Stage::BATTLE)
        return;

    int search_distance = 40;

    if (is_obj_in_distance(state.ultra_info, search_distance))
    {
        if (is_adjusting_needed(state.ultra_info, 1))
        {
            car_adjust_attack_direction(state.ultra_info, 160);
            delay(TIMESLICE * 3);
        }
        else
        {
            car_go_forward(160);
            delay(TIMESLICE * 3);
        }
    }
    else
    {
        search_strategy(Strategy::ROTATION, search_distance, 130, 500);
    }
}

void task_oled_display(MK2System::VehState &state)
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    // Display static text
    display.print("stage:");
    switch (state.stage)
    {
    case MK2System::Stage::INIT:
        display.println("INIT");
        break;
    case MK2System::Stage::BATTLE:
        display.println("Battle");
        break;
    case MK2System::Stage::BACKWARD:
        display.println("Backward");
        break;
    default:
        break;
    }

    display.print("L:");
    display.println(state.ultra_info.left_sensor.getValue());
    display.print("R:");
    display.println(state.ultra_info.right_sensor.getValue());

    display.print("Edge:");
    if (state.edge_info.hasValue())
    {
        display.println("Inside");
    }
    else
    {
        switch (state.edge_info.getValue())
        {
        case Edge_Signal::BACK:
            display.println("Back");
            break;
        case Edge_Signal::FRONT:
            display.println("Front");
            break;
        case Edge_Signal::FRONT_LEFT:
            display.println("Front Left");
            break;
        case Edge_Signal::FRONT_RIGHT:
            display.println("Front Right");
            break;
        default:
            break;
        }
    }

    display.display();
}

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

    MK2System::init();
    MK2System::register_task(task_searching);
    MK2System::register_task(task_qtr);
    MK2System::register_task(task_backward);
    MK2System::register_task(task_normal_attack);
    MK2System::register_task(task_oled_display);

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
            if (is_adjusting_needed(info, tolerance))
            {
                car_adjust_attack_direction(info, speed);
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
        MK2System::run();
    }
};

void loop()
{
    Serial.println("===============");
    Test::normal_mode();
    delay(500);
}