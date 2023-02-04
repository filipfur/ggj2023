#include "clientsession.h"

void ClientSession::update(float dt)
{
    auto it = _scheduledTasks.begin();
    while(it != _scheduledTasks.end())
    {
        if(it->tick(dt))
        {
            it = _scheduledTasks.erase(it);
        }
        else
        {
            ++it;
        }
    }
    glm::vec3 delta{wasdToDelta(wasd())};
    if(dead())
    {
        waitToRespawn(dt, 2);
    }
    else
    {
        if(state() == 0) // If actionId doing nothing.
        {
            _clientState->xrz.y = _clientInput->aimDirection;
            while(_clientState->xrz.y > 180.0f) { _clientState->xrz.y -= 360.0f; }
            while(_clientState->xrz.y < -180.0f) { _clientState->xrz.y += 360.0f; }
            if(delta.x * delta.x > 0.01f)
            {
                //_clientState->xrz.y -= 1.57f * delta.x * dt;
                float distance = 8.0f * delta.x * dt;
                float alpha = glm::radians(_clientState->xrz.y - 90.0f);
                _clientState->xrz.x += glm::sin(alpha) * distance;
                _clientState->xrz.z += glm::cos(alpha) * distance;
            }
            if(delta.z * delta.z > 0.01f)
            {
                float distance = 8.0f * delta.z * dt;
                float alpha = glm::radians(_clientState->xrz.y);
                _clientState->xrz.x += glm::sin(alpha) * distance;
                _clientState->xrz.z += glm::cos(alpha) * distance;
            }
            uint8_t aId{actionId()};
            if(aId > 0 && aId < 0xA) // TODO: Implement generics and specifics 8)
            {
                /*switch(characterId())
                {
                    case 1:
                        handleSwagBotAction(aId);
                        break;
                    case 2:
                        handleMiniTankAction(aId);
                        break;
                    case 3:
                        handleMonarchAction(aId);
                        break;
                }*/
                float stateDuration = 0.0f;
                switch(aId)
                {
                    case letsgetsocial::ACTION_KEY_SPACE:
                        _state = 0x1;
                        stateDuration = 1.0f;
                        /*_scheduledTasks.push_back(ScheduledTask{0.800f, [this](){
                            
                        }});*/
                        break;
                    case letsgetsocial::ACTION_KEY_Q:
                        _state = 0x2;
                        //stateDuration = 1.0f;
                        _moveable = false;
                        _scheduledTasks.push_back(ScheduledTask{1.5f, [this](){
                            _moveable = true;
                        }});
                        return;
                        break;
                }
                if(stateDuration == 0.0f)
                {
                    _state = 0x0;
                }
                else
                {
                    _scheduledTasks.push_back(ScheduledTask{stateDuration, [this](){
                        _state = 0x0;
                    }});
                }
            }
            else if(aId > 0x9)
            {
                std::cout << "Setting character id: " << (int)(aId - 0x9) << std::endl;
                setCharacterId(aId - 0x9);
            }
        }
        else if(_state == 0x2)
        {
            if(_moveable && (delta.x != 0.0f || delta.z != 0.0f))
            {
                _state = 0x0;
            }
        }
    }
}