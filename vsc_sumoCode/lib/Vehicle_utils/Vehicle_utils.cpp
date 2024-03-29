#include <Arduino.h>
#include <Configs.h>
#include <Tools.hpp>
#include <Debug.h>

#include "Vehicle_utils.hpp"

void wheel_forward(uint8_t forward_pin, uint8_t backward_pin)
{
    digitalWrite(forward_pin, HIGH);
    digitalWrite(backward_pin, LOW);
}

void wheel_backward(uint8_t forward_pin, uint8_t backward_pin)
{
    digitalWrite(forward_pin, LOW);
    digitalWrite(backward_pin, HIGH);
}

void wheel_stop(uint8_t forward_pin, uint8_t backward_pin)
{
    digitalWrite(forward_pin, HIGH);
    digitalWrite(backward_pin, HIGH);
}

void wheel_move_speed(uint8_t pin, int speed)
{
    analogWrite(pin, speed);
}

void car_go_forward(int speed)
{
    wheel_move_speed(LEFT_MOTOR, speed-DELTA);
    wheel_move_speed(RIGHT_MOTOR, speed+DELTA);
    wheel_forward(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
    wheel_forward(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
}

void car_go_forward_by_speed(int left_wheel_speed, int right_wheel_speed)
{
    wheel_move_speed(LEFT_MOTOR, left_wheel_speed);
    wheel_move_speed(RIGHT_MOTOR, right_wheel_speed);
    wheel_forward(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
    wheel_forward(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
}

void car_go_backward(int speed)
{
    wheel_move_speed(LEFT_MOTOR, speed-DELTA);
    wheel_move_speed(RIGHT_MOTOR, speed+DELTA);
    wheel_backward(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
    wheel_backward(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
}

void car_go_backward_by_speed(int left_wheel_speed, int right_wheel_speed)
{
    wheel_move_speed(LEFT_MOTOR, left_wheel_speed);
    wheel_move_speed(RIGHT_MOTOR, right_wheel_speed);
    wheel_backward(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
    wheel_backward(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
}

void car_turn_right_by_speed(int left_wheel_forward_speed, int right_wheel_backward_speed)
{
    wheel_move_speed(LEFT_MOTOR, left_wheel_forward_speed);
    wheel_move_speed(RIGHT_MOTOR, right_wheel_backward_speed);
    wheel_forward(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
    wheel_backward(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
}

void car_turn_left_by_speed(int left_wheel_backward_speed, int right_wheel_forward_speed)
{
    wheel_move_speed(LEFT_MOTOR, left_wheel_backward_speed);
    wheel_move_speed(RIGHT_MOTOR, right_wheel_forward_speed);
    wheel_forward(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
    wheel_backward(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
}

void car_turn_left(int speed)
{
    car_turn_left_by_speed(speed, 0);
}

void car_turn_right(int speed)
{
    car_turn_right_by_speed(0, speed);
}

void car_turn_left_by_degree(double degree)
{
    wheel_move_speed(LEFT_MOTOR, 0);
    wheel_move_speed(RIGHT_MOTOR, 75);
    wheel_forward(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
    wheel_backward(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
    delay(ROTATE_ONE_DEGREE_IN_MS * degree);
}

void car_turn_right_by_degree(double degree)
{
    wheel_move_speed(LEFT_MOTOR, 75);
    wheel_move_speed(RIGHT_MOTOR, 0);
    wheel_forward(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
    wheel_backward(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
    delay(ROTATE_ONE_DEGREE_IN_MS * degree);
}

void car_go_random(int speed)
{
    long rand_num = random(3);
    switch (rand_num)
    {
    case 0:
        car_go_forward(speed);
        break;
    case 1:
        car_turn_left(speed);
        break;
    case 2:
        car_turn_right(speed);
        break;
    }
}

TupleMut<OP_Vehicle, int> car_adjustment_measurement(double left_sensor, double right_sensor)
{
    int gap = left_sensor - right_sensor;
    if (gap > 0)
        return TupleMut<OP_Vehicle, int>(OP_Vehicle::GO_RIGHT, gap);

    gap = abs(gap);
    return TupleMut<OP_Vehicle, int>(OP_Vehicle::GO_LEFT, gap);
}

int car_adjust_attack_direction(Obj_direction info, int speed)
{
    int actual_speed = speed;
    if (info.left_sensor.hasValue() && info.right_sensor.hasValue())
    {
        auto result = car_adjustment_measurement(info.left_sensor.getValue(), info.right_sensor.getValue());
        // double factor = (abs(result.snd) - tolerance) / result.snd;

        // double factor = curve_algorithm(1);
        double factor = 1;
        actual_speed = speed * factor;

        switch (result.fst)
        {
        case OP_Vehicle::GO_LEFT:
            car_turn_left_by_speed(actual_speed, actual_speed);
            break;
        case OP_Vehicle::GO_RIGHT:
            car_turn_right_by_speed(actual_speed, actual_speed);
            break;
        default:
            break;
            ;
        }
    }
    else if (info.left_sensor.hasValue())
    {
        // double factor = curve_algorithm(1);
        double factor = 1;
        actual_speed = speed * factor;
        car_turn_left_by_speed(actual_speed, actual_speed);
    }
    else if (info.right_sensor.hasValue())
    {
        // double factor = curve_algorithm(1);
        double factor = 1;
        actual_speed = speed * factor;
        car_turn_right_by_speed(actual_speed, actual_speed);
    }

    return actual_speed;
}

void car_stop(void)
{
    wheel_stop(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
    wheel_stop(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
}

Maybe<double> detect_obj_distance(uint8_t trigger_pin, uint8_t echo_pin)
{
    digitalWrite(trigger_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigger_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger_pin, LOW);

    auto val = pulseIn(echo_pin, HIGH, MAX_DISTANCE * 58) / 58.0;
    return val != 0 ? Maybe(val) : Maybe<double>::Nothing();
}

Maybe<Edge_Signal> determine_edge(uint8_t qtr_sensor_front_left, uint8_t qtr_sensor_front_right,
                                  uint8_t qtr_sensor_back)
{
    int frontL = 0;
    int frontR = 0;
    int back = 0;

    for (int i = 0; i <= MAX_QTR_SCAN_TIME; i++)
    {
        frontL += analogRead(qtr_sensor_front_left);
        frontR += analogRead(qtr_sensor_front_right);
        back += analogRead(qtr_sensor_back);
    }
    frontL /= MAX_QTR_SCAN_TIME;
    frontR /= MAX_QTR_SCAN_TIME;
    back /= MAX_QTR_SCAN_TIME;

    Serial.println((String(frontL)).c_str());
    Serial.println((String(frontR)).c_str());
    Serial.println((String(back)).c_str());

    // TODO: change back
    if (frontL <= QTR_THRESHOLD && frontR <= QTR_THRESHOLD)
    {
        return Maybe(Edge_Signal::FRONT);
    }
    else if (frontL <= QTR_THRESHOLD)
    {
        return Maybe(Edge_Signal::FRONT_LEFT);
    }
    else if (frontR <= QTR_THRESHOLD)
    {
        return Maybe(Edge_Signal::FRONT_RIGHT);
    }
    else if (back <= QTR_THRESHOLD)
    {
        return Maybe(Edge_Signal::BACK);
    }
    else
    {
        return Maybe<Edge_Signal>::Nothing();
    }
}

bool is_obj_in_distance(Obj_direction info, double range)
{
    return (info.left_sensor.hasValue() && info.left_sensor.getValue() <= range) ||
           (info.right_sensor.hasValue() && info.right_sensor.getValue() <= range);
}

// range ~= 60
Obj_direction obj_detection_info()
{
    auto distance_l = detect_obj_distance(TRIGGER_PIN_L, ECHO_PIN_L);
    auto distance_r = detect_obj_distance(TRIGGER_PIN_R, ECHO_PIN_R);

    return (Obj_direction){
        .left_sensor = distance_l,
        .right_sensor = distance_r,
    };
}

bool is_adjusting_needed(Obj_direction info, double tolerance)
{
    if (info.left_sensor.hasValue() && info.right_sensor.hasValue())
    {
        auto lv = info.left_sensor.getValue();
        auto rv = info.right_sensor.getValue();

        if (abs(lv - rv) > tolerance)
        {
            return true;
        }
    }
    else if (info.left_sensor.hasValue() || info.right_sensor.hasValue())
    {
        return true;
    }
    return false;
}

double calculate_gap(Obj_direction info)
{
    if (info.left_sensor.hasValue() && info.right_sensor.hasValue())
    {
        auto lv = info.left_sensor.getValue();
        auto rv = info.right_sensor.getValue();
        // TODO: need an algorithm
        double x = 3;

        return x;
    }
    else if (info.left_sensor.hasValue() || info.right_sensor.hasValue())
    {
        return -1;
    }

    return -1;
}