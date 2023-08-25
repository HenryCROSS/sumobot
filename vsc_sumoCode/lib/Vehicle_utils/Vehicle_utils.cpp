#include <Arduino.h>
#include <Configs.h>

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
    digitalWrite(forward_pin, LOW);
    digitalWrite(backward_pin, LOW);
}

void wheel_move_speed(uint8_t pin, int speed)
{
    analogWrite(pin, speed);
}

void car_go_forward(int speed)
{
    wheel_move_speed(LEFT_MOTOR, speed);
    wheel_move_speed(RIGHT_MOTOR, speed);
    wheel_forward(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
    wheel_forward(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
}

void car_go_backward(int speed)
{
    wheel_move_speed(LEFT_MOTOR, speed);
    wheel_move_speed(RIGHT_MOTOR, speed);
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

TupleMut<OP_Vehicle, int> car_adjustment_measurement(Obj_direction info, int tolerance)
{
    if (info.left_sensor.hasValue() && info.right_sensor.hasValue())
    {
        int gap = info.left_sensor.getValue() - info.right_sensor.getValue();
        if (gap > tolerance)
            return TupleMut<OP_Vehicle, int>(OP_Vehicle::GO_RIGHT, gap);

        gap = abs(gap);
        if (gap > tolerance)
            return TupleMut<OP_Vehicle, int>(OP_Vehicle::GO_LEFT, gap);
    }

    return TupleMut<OP_Vehicle, int>(OP_Vehicle::GO_FORWARD, 0);
}

void car_adjust_attack_direction(Obj_direction info, double tolerance, int speed)
{
    auto result = car_adjustment_measurement(info, tolerance);
    double factor = (abs(result.snd) - tolerance) / result.snd;

    switch (result.fst)
    {
    case OP_Vehicle::GO_LEFT:
        car_turn_left(speed * factor);
        break;
    case OP_Vehicle::GO_RIGHT:
        car_turn_right(speed * factor);
        break;
    default:
        break;
        ;
    }
}

void car_stop(void)
{
    wheel_stop(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
    wheel_stop(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
}

double detect_obj_distance(uint8_t trigger_pin, uint8_t echo_pin)
{
    digitalWrite(trigger_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigger_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger_pin, LOW);
    return pulseIn(echo_pin, HIGH, MAX_DISTANCE * 58) / 58.0;
}

Maybe<Edge_Signal> determine_edge(uint8_t qtr_sensor_front_left, uint8_t qtr_sensor_front_right, uint8_t qtr_sensor_back)
{
    int frontL = analogRead(qtr_sensor_front_left);
    int frontR = analogRead(qtr_sensor_front_right);
    // int back = analogRead(qtr_sensor_back);
    // int frontR = 0;
    int back = 0;
    Serial.print("FrontL: ");
    // Serial.println(frontL);

    Serial.print(frontL);
    Serial.print(":L /");
    Serial.print(frontR);
    Serial.print(":R /");
    Serial.print(back);
    Serial.print(":B /");
    Serial.println("Front");
    if (frontL <= 400 && frontR <= 400)
    {
        return Maybe(Edge_Signal::FRONT);
    }
    else if (frontL <= 400)
    {
        // Serial.print(frontR);
        // Serial.println("FrontLeft");
        return Maybe(Edge_Signal::FRONT_LEFT);
    }
    else if (frontR <= 400)
    {
        // Serial.print(frontR);
        // Serial.println("FrontRight");
        return Maybe(Edge_Signal::FRONT_RIGHT);
    }
    else if (back <= 400)
    {
        // Serial.print(back);
        // Serial.println("Back");
        return Maybe(Edge_Signal::BACK);
    }
    else
    {
        // Serial.println("None");
        return Maybe<Edge_Signal>::Nothing();
    }
}

bool is_obj_in_distance(Obj_direction info, double range)
{
    return (info.left_sensor.hasValue() && info.left_sensor.getValue() <= range) || (info.right_sensor.hasValue() && info.right_sensor.getValue() <= range);
}

// range ~= 60
Obj_direction obj_detected_info(double range)
{
    double distance_l = detect_obj_distance(TRIGGER_PIN_L, ECHO_PIN_L);
    double distance_r = detect_obj_distance(TRIGGER_PIN_R, ECHO_PIN_R);

    return (Obj_direction){
        .left_sensor = distance_l < range ? Maybe<double>(distance_l) : Maybe<double>::Nothing(),
        .right_sensor = distance_r < range ? Maybe<double>(distance_r) : Maybe<double>::Nothing(),
    };
}

bool is_adjusting_needed(Obj_direction info, double max_range, double tolerance)
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
    return false;
}