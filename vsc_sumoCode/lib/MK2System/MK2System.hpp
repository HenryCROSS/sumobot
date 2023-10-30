#ifndef _MK2SYSTEM_H_
#define _MK2SYSTEM_H_

#include <Configs.h>
#include <Tools.hpp>
#include <Vehicle_actions.hpp>
#include <Vehicle_types.hpp>

class MK2System
{
public:
    using Task = void (*)(void);

    static void init();
    static void run();
    static int register_task(const Task &task);

private:
    static Array<Task, MAX_TASKS> tasks;
    static VehState state;
};

#endif