#ifndef _VEHICLE_ACTIONS_HPP_
#define _VEHICLE_ACTIONS_HPP_

#include <Vehicle_utils.hpp>
#include <Tools.hpp>

enum class Strategy
{
    ROTATION,
};

bool search_strategy(Strategy strategy, int distance, int speed, unsigned long ms);

#endif // !_VEHICLE_ACTIONS_HPP_
