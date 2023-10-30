#ifndef _VEHICLE_TYPES_HPP_
#define _VEHICLE_TYPES_HPP_

#include <Tools.hpp>
#include <Vehicle_actions.hpp>
#include <Vehicle_types.hpp>

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

enum class Stage
{
    INIT,
    BATTLE,
};

enum class VehMotion
{
    TURNING,
    FORWARD,
    BACKWARD,
    SEARCH,
    ADJUST,
    VOID,
};

struct VehState
{
    Stage stage;
    Strategy search_strategy;
    VehMotion motion;
    int speed;
    int range;
    Obj_direction ultra_info;
    Maybe<Edge_Signal> edge_info;

    VehState()
    {
        range = 0;
        speed = 0;
        stage = Stage::INIT;
        edge_info = Maybe<Edge_Signal>::Nothing();
        search_strategy = Strategy::ROTATION;
        motion = VehMotion::VOID;
        ultra_info = {
            Maybe<double>::Nothing(),
            Maybe<double>::Nothing()};
    }
};

#endif // !_VEHICLE_TYPES_HPP_
