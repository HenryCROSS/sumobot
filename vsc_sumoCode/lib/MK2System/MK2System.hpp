#ifndef _MK2SYSTEM_H_
#define _MK2SYSTEM_H_

#include <Configs.h>
#include <Tools.hpp>
#include <Vehicle_types.hpp>

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
        int speed;
        int range;
        Obj_direction ultra_info;
        uint32_t custom_flag;
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