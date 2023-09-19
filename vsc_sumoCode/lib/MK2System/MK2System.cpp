#include "MK2System.hpp"

MK2System::VehState MK2System::state;

void MK2System::init()
{
    state.custom_flag = 0;
    state.range = 0;
    state.speed = 0;
    state.stage = Stage::INIT;
    state.ultra_info = {
        Maybe<double>::Nothing(),
        Maybe<double>::Nothing()};
}

void MK2System::run()
{
    switch (state.stage)
    {
    case Stage::INIT:
        state.stage = Stage::BACKWARD;
        break;

    default:
        for (int i = 0; i < tasks.size(); i++)
        {
            tasks[i](state);
        }

        break;
    }
}

bool MK2System::register_task(const Task &task)
{
    if (tasks.size() >= tasks.capacity())
    {
        return false;
    }

    tasks.push_back(task);
    return true;
}