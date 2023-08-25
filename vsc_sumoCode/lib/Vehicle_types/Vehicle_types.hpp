#ifndef _VEHICLE_TYPES_HPP_
#define _VEHICLE_TYPES_HPP_

#include <Tools.hpp>

enum class Edge_Signal
{
    FRONT,
    FRONT_RIGHT,
    FRONT_LEFT,
    BACK,
};

enum class OP_Vehicle
{
    GO_LEFT,
    GO_RIGHT,
    GO_FORWARD,
    GO_BACKWARD,
};

struct Obj_direction
{
    Maybe<double> left_sensor;
    Maybe<double> right_sensor;
};

#endif // !_VEHICLE_TYPES_HPP_
