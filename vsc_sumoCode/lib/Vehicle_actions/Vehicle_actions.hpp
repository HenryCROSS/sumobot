#ifndef _VEHICLE_ACTIONS_HPP_
#define _VEHICLE_ACTIONS_HPP_

enum class Strategy
{
    ROTATION,
};

bool search_strategy(Strategy strategy, int distance, int speed, unsigned long ms);

bool attack_strategy(int speed, unsigned long ms);

#endif // !_VEHICLE_ACTIONS_HPP_
