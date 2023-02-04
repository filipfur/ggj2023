#pragma once

#include <functional>

using Callback = std::function<void()>;

class ScheduledTask
{
public:
    ScheduledTask(float time, Callback callback) : _time{time}, _callback{callback}
    {

    }

    bool tick(float dt)
    {
        if(_time > 0)
        {
            _time -= dt;
            if(_time <= 0)
            {
                _callback();
                return true;
            }
        }
        return false;
    }

private:
    Callback _callback;
    float _time;
};