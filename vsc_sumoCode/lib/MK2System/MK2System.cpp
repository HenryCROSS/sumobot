#include "MK2System.hpp"

Array<MK2System::Task, MAX_TASKS> MK2System::tasks;

void MK2System::init()
{
}

void MK2System::run()
{
    for (int i = 0; i < tasks.size(); i++)
    {
        tasks[i]();
    }
}

int MK2System::register_task(const Task &task)
{
    if (tasks.size() >= tasks.capacity())
    {
        return -1;
    }

    tasks.push_back(task);
    return tasks.size() - 1;
}