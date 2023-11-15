#include <Arduino.h>
// #include <OLED_utils.hpp>
#include <Configs.h>
#include <Debug.h>
#include <MK2System.hpp>
#include <Vehicle_actions.hpp>
#include <Vehicle_utils.hpp>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 1);

VehState g_state{};
volatile bool front_left = false;
volatile bool front_right = false;

void qtr_left_interrupt(){
    front_left = true;
}
void qtr_right_interrupt(){
    front_right = true;
}

void task_searching()
{
    g_state.ultra_info = obj_detection_info();
}

void task_qtr()
{
    g_state.edge_info = determine_edge(QTR_SENSOR_FL, QTR_SENSOR_FR, QTR_SENSOR_B);
}

void task_edge_action(){

}

void task_normal_attack()
{
    // it is useful when more than 1 mode
    // if (g_state.stage != Stage::BATTLE)
    //     return;

    int search_distance = 40;

    if (front_left || front_right ||  g_state.edge_info.hasValue())
    {
        g_state.motion = VehMotion::TURNING;
        // switch (g_state.edge_info.getValue())
        // {
        // case Edge_Signal::BACK:
        //     // TODO: maybe go random?
        //     car_go_forward(SPEED);
        //     break;
        // case Edge_Signal::FRONT:
        //     car_turn_left_by_speed(SPEED, SPEED);
        //     break;
        // case Edge_Signal::FRONT_LEFT:
        //     car_turn_left_by_speed(SPEED, SPEED);
        //     break;
        // case Edge_Signal::FRONT_RIGHT:
        //     car_turn_right_by_speed(SPEED, SPEED);
        //     break;
        // default:
        //     break;
        // }
        
        if(front_left && front_right){
            car_go_backward(SPEED);
            delay(200);
            car_turn_left_by_speed(SPEED, SPEED);
            delay(200);
        }else if (front_left){
            car_turn_left(SPEED);
            delay(200);
        }else if (front_right){
            car_turn_right(SPEED);
            delay(200);
        }else {
            car_go_forward(SPEED);
            delay(200);
        }

        front_left = false;
        front_right = false;

        g_state.speed = SPEED;
    }
    else if (is_obj_in_distance(g_state.ultra_info, search_distance))
    {
        g_state.motion = VehMotion::FORWARD;

        // calculation of the gap
        // auto gap = abs(g_state.ultra_info.left_sensor.getValue() - g_state.ultra_info.left_sensor.getValue());
        auto gap = calculate_gap(g_state.ultra_info);

        if (is_adjusting_needed(g_state.ultra_info, gap))
        {
            g_state.motion = VehMotion::ADJUST;
            g_state.speed = car_adjust_attack_direction(g_state.ultra_info, SPEED);
            delay(TIMESLICE * 3);
        }
        else
        {
            // attack strategy
            if (is_obj_in_distance(g_state.ultra_info, 10))
            {
                // attack_strategy(120, TIMESLICE * 10);
                car_go_forward(255);
                g_state.speed = 255;
            }
            else if (is_obj_in_distance(g_state.ultra_info, 20))
            {
                // attack_strategy(120, TIMESLICE * 10);
                car_go_forward(155);
                g_state.speed = 155;
            }
            else
            {
                car_go_forward(90);
                g_state.speed = 90;
                delay(TIMESLICE * 3);
            }
        }
    }
    else
    {
        g_state.motion = VehMotion::SEARCH;
        search_strategy(g_state.search_strategy, search_distance, SPEED, 500);
        // car_go_forward(SPEED);
        g_state.speed = SPEED;
    }
}

void task_oled_display()
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    // Display static text
    display.print("stage: ");
    switch (g_state.stage)
    {
    case Stage::INIT:
        display.println("INIT");
        break;
    case Stage::BATTLE:
        display.println("Battle");
        break;
    default:
        break;
    }

    display.print("L: ");
    display.println(g_state.ultra_info.left_sensor.getValue());
    display.print("R: ");
    display.println(g_state.ultra_info.right_sensor.getValue());

    display.print("Speed: ");
    display.println(g_state.speed);

    display.print("Edge: ");
    if (g_state.edge_info.hasValue())
    {
        switch (g_state.edge_info.getValue())
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
    else
    {
        display.println("Void");
    }

    display.print("Motion: ");

    switch (g_state.motion)
    {
    case VehMotion::VOID:
        display.println("Void");
        break;
    case VehMotion::TURNING:
        display.println("Turing");
        break;
    case VehMotion::FORWARD:
        display.println("Forward");
        break;
    case VehMotion::BACKWARD:
        display.println("Backward");
        break;
    case VehMotion::ADJUST:
        display.println("Adjust");
        break;
    case VehMotion::SEARCH:
        display.println("Search");
    default:
        break;
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

    pinMode(1, INPUT_PULLUP);
    pinMode(2, INPUT_PULLUP);

    MK2System::init();
    MK2System::register_task(task_searching, Task_Type::PREEMPTIVE, -1);
    // MK2System::register_task(task_qtr, Task_Type::PREEMPTIVE, -1);
    MK2System::register_task(task_normal_attack, Task_Type::PREEMPTIVE, -1);

    attachInterrupt(digitalPinToInterrupt(1), qtr_left_interrupt, RISING);
    attachInterrupt(digitalPinToInterrupt(2), qtr_right_interrupt, RISING);

    if (display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    { // Address 0x3D for 128x64
        MK2System::register_task(task_oled_display, Task_Type::PREEMPTIVE, -1);
        delay(2000);

        g_state.monitor_running = true;

        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0, 10);
        // Display static text
        display.println("CHARGE!!");
        display.display();
    }
    else
    {
        Serial.println(F("SSD1306 allocation failed"));
        delay(2000);
    }

    delay(4000);
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
            debug::serial_println("VehAction: NOT MOVE");
        }

        delay(delay_ms);
    }

    static void monitor_test_mode()
    {
        if (g_state.monitor_running)
        {
            for (int i = 0; i < 100; i++)
            {
                display.clearDisplay();
                display.setTextSize(2);
                display.setTextColor(WHITE);
                display.setCursor(0, 10);
                // Display static text
                display.println(i);
                display.display();
                delay(500);
            }
        }
    }

    static void normal_mode()
    {
        MK2System::run();
    }
};

void loop()
{
    debug::serial_println("===============");
    Test::normal_mode();

    // Test::monitor_test_mode();
    // attack_strategy(100, 100);

    // car_turn_left_by_speed(SPEED, 0);
    // delay(480);

    // car_stop();
    // delay(1000);
}