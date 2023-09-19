#include "MK2System.hpp"

void MK2System::run()
{
    switch (state.stage)
    {
    case Stage::INIT:
        state.stage = Stage::BACKWARD;
        break;

    default:
        for(int8_t i = 0; i < MAX_TASKS; i++)
        {
            tasks[i](state);
        }

        break;
    }
}

bool MK2System::register_task(const Task& task)
{
    if(tasks.size() >= tasks.capacity()){
        return false;
    }

    tasks.push_back(task);
    return true;
}