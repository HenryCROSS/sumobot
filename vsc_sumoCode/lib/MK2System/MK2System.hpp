#ifndef _MK2SYSTEM_H_
#define _MK2SYSTEM_H_

#include <Configs.h>
#include <Tools.hpp>
#include <Vehicle_actions.hpp>
#include <Vehicle_types.hpp>

enum class VehMotion
{
    TURNING,
    FORWARD,
    BACKWARD,
    SEARCH,
    VOID,
};

class MK2System
{
public:
    enum class Stage
    {
        INIT,
        BATTLE,
        BACKWARD,
    };

    struct VehState
    {
        Stage stage;
        Strategy search_strategy;
        VehMotion motion;
        int speed;
        int range;
        uint32_t custom_flag;
        Obj_direction ultra_info;
        Maybe<Edge_Signal> edge_info;
    };

    using Task = void (*)(VehState &);

    static void init();
    static void run();
    static bool register_task(const Task &task);

private:
    static Array<Task, MAX_TASKS> tasks;
    static VehState state;
};

#endif