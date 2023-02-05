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
        _state = letsgetsocial::ClientActionState::DEATH;
        waitToRespawn(dt, 2);
    }
    else
    {
        switch (state())
        {
        case letsgetsocial::ClientActionState::IDLE: // If actionId doing nothing.
        {
            _clientState->xrz.y = _clientInput->aimDirection;
            while(_clientState->xrz.y > 180.0f) { _clientState->xrz.y -= 360.0f; }
            while(_clientState->xrz.y < -180.0f) { _clientState->xrz.y += 360.0f; }
            if(delta.x * delta.x > 0.01f)
            {
                //_clientState->xrz.y -= 1.57f * delta.x * dt;
                float distance = 12.0f * delta.x * dt;
                float alpha = glm::radians(_clientState->xrz.y - 90.0f);
                _clientState->xrz.x += glm::sin(alpha) * distance;
                _clientState->xrz.z += glm::cos(alpha) * distance;
            }
            if(delta.z * delta.z > 0.01f)
            {
                float distance = 12.0f * delta.z * dt;
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
                        _state = letsgetsocial::ClientActionState::HEAD_BUTT;
                        stateDuration = 1.0f;
                        break;
                    case letsgetsocial::ACTION_KEY_Q:
                        _state = letsgetsocial::ClientActionState::DIG;
                        stateDuration = 1.5f;
                        _moveable = false;
                        _scheduledTasks.push_back(ScheduledTask{stateDuration, [this](){
                            _moveable = true;
                        }});
                        return;
                }
                if(stateDuration == 0.0f)
                {
                    _state = letsgetsocial::ClientActionState::IDLE;
                }
                else
                {
                    _scheduledTasks.push_back(ScheduledTask{stateDuration, [this](){
                        _state = letsgetsocial::ClientActionState::IDLE;
                    }});
                }
            }
            else if(aId > 0x9)
            {
                std::cout << "Setting character id: " << (int)(aId - 0x9) << std::endl;
                setCharacterId(aId - 0x9);
            }
            break;
        }
        case letsgetsocial::ClientActionState::DIG:
            if(_moveable && (delta.x * delta.x > 0.01f || delta.z * delta.z > 0.01f))
            {
                _state = letsgetsocial::ClientActionState::IDLE;
            }
            break;
        case letsgetsocial::ClientActionState::AIRBORNE:
            _clientState->xrz.x += _headButtVelocity.x * dt;
            _clientState->xrz.z += _headButtVelocity.y * dt;
            break;
        }
    }
}