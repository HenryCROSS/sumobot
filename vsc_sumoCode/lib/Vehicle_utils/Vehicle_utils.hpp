#ifndef _VEHICLE_UTILS_HPP_
#define _VEHICLE_UTILS_HPP_

#include <Arduino.h>
#include <Vehicle_types.hpp>

TupleMut<OP_Vehicle, int> car_adjustment_measurement(double left_sensor, double right_sensor);

void wheel_forward(uint8_t forward_pin, uint8_t backward_pin);

void wheel_backward(uint8_t forward_pin, uint8_t backward_pin);

void wheel_stop(uint8_t forward_pin, uint8_t backward_pin);

void wheel_move_speed(uint8_t pin, int speed);

void car_go_forward(int speed);

void car_go_backward(int speed);

void car_turn_right_by_speed(int left_wheel_forward_speed, int right_wheel_backward_speed);

void car_turn_left_by_speed(int left_wheel_backward_speed, int right_wheel_forward_speed);

void car_turn_left(int speed);

void car_turn_right(int speed);

void car_go_random(int speed);

void car_adjust_attack_direction(Obj_direction info, double tolerance, int speed);

void car_stop(void);

Maybe<double> detect_obj_distance(uint8_t trigger_pin, uint8_t echo_pin);

Maybe<Edge_Signal> determine_edge(uint8_t qtr_sensor_front_left, uint8_t qtr_sensor_front_right, uint8_t qtr_sensor_back);

bool is_obj_in_distance(Obj_direction info, double range);

// range ~= 60
Obj_direction obj_detection_info();

bool is_adjusting_needed(Obj_direction info, double max_range, double tolerance);

#endif // !_VEHICLE_UTILS_HPP_