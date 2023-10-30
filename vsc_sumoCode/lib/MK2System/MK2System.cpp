#include "MK2System.hpp"

Array<Task, MAX_TASKS> MK2System::tasks;

void MK2System::init()
{
}

void MK2System::run()
{
    for (int i = 0; i < tasks.size(); i++)
    {
        auto task = tasks[i];
        if (task.type == Task_Type::PREEMPTIVE)
        {
            task.fn();
        }
        else
        {
            auto current_time = millis();
            if ((task.life == -1 || task.life > 0) &&
                ((current_time - task.prev_time) >= task.interval ||
                 (!task.start && (current_time - task.prev_time) >= task.delay && (task.start = true))))
            {
                task.life = task.life > 0 ? (task.life - 1) : -1;
                task.prev_time = current_time;
                task.fn();
            }
        }
    }
}

int MK2System::register_task(const Task_Fn fn, Task_Type type, int life, uint64_t delay, uint64_t interval)
{
    if (tasks.size() >= tasks.capacity())
    {
        return -1;
    }

    auto task = Task{
        .start = false,
        .life = life,
        .fn = fn,
        .type = type,
        .delay = delay,
        .interval = interval,
        .prev_time = millis(),
    };

    tasks.push_back(task);
    return tasks.size() - 1;
}