#ifndef _MK2SYSTEM_H_
#define _MK2SYSTEM_H_

#include <Configs.h>
#include <Tools.hpp>
#include <Vehicle_actions.hpp>
#include <Vehicle_types.hpp>

struct TaskIdTag{};
using TaskId = NewType<int, TaskIdTag>;

enum class Task_Type
{
    PREEMPTIVE,
    COOPERATIVE,
};

using Task_Fn = void (*)(void);

struct Task
{
    bool start;
    int life; // -1 = forever or positive n times
    Task_Fn fn;
    Task_Type type;
    uint64_t delay;
    uint64_t interval;
    uint64_t prev_time;
};

class MK2System
{
public:
    static void init();
    static void run();
    // return task id
    static TaskId register_task(Task_Fn fn, Task_Type type, int life, uint64_t delay = 0, uint64_t interval = 0);

private:
    static Array<Task, MAX_TASKS> tasks;
    static VehState state;
};

#endif