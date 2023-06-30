#ifndef STRATEGY_H_
#define STRATEGY_H_

bool is_obj_exist(double range)
{
    double distance = detect_obj_distance(TRIGGER_PIN, ECHO_PIN);

    if (distance < range)
    {
        return true;
    }
    else
    {
        return false;
    }
}

#endif // STRATEGY_H_
