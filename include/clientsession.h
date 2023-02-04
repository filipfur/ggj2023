#pragma once 

#include <algorithm>
#include <chrono>
#include <map>

#include "lgscore.h"
#include "scheduledtask.h"

#include "circlebb.h"

class ClientSession
{
public:
    ClientSession(uint8_t id, letsgetsocial::ClientInput* clientInput, letsgetsocial::ClientState* clientState, SOCKADDR_IN endpoint)
        : _id{id}, _clientInput{clientInput}, _clientState{clientState}, _endpoint{endpoint}, _boundingBox{glm::vec3{clientState->xrz.x, 0.0f, clientState->xrz.z}, 3.0f}
    {
        refresh();
    }

    virtual ~ClientSession()
    {
        _clientInput = nullptr;
        _clientState = nullptr;
    }

    letsgetsocial::ClientInput* clientInput() const
    {
        return _clientInput;
    }

    letsgetsocial::ClientState* clientState() const
    {
        return _clientState;
    }

    SOCKADDR_IN& endpoint()
    {
        return _endpoint;
    }

    uint8_t id() const
    {
        return _id;
    }

    void refresh()
    {
        _lastUpdate = std::chrono::steady_clock::now();
    }

    std::chrono::steady_clock::time_point lastUpdate() const
    {
        return _lastUpdate;
    }

    glm::vec3 wasdToDelta(uint8_t wasd)
    {
        float x{0.0f};
        float z{0.0f};
        if(wasd & 0b1000) // W
        {
            z = 1.0f;
        }
        else if(wasd & 0b0010) // S
        {
            z = -1.0f;
        }

        if(wasd & 0b0100) // A
        {
            x = -1.0f;
        }
        else if(wasd & 0b0001) // D
        {
            x = 1.0f;
        }
        return glm::normalize(glm::vec3{x, 0.0f, z});
    }

    virtual void update(float dt);

    void updateClientData()
    {
        _clientState->clientData = (characterId() << 28) & 0xF0000000
            | (_state << 24) & 0x0F000000
            | (_health << 16) & 0x00FF0000
            | ((_clientInput->control & 0xF) << 12) & 0x0000F000
            | ((int)(_clientInput->aimDirection / 360.0f * 4095) & 0x00000FFF);
    }

    uint8_t health() const
    {
        return _health;
    }

    void setHealth(uint8_t health)
    {
        _health = health;
    }

    void waitToRespawn(float dt, uint8_t maxHealth)
    {
        _respawnTimer -= dt;
        if(_respawnTimer <= 0)
        {
            _health = maxHealth;
            _clientState->xrz.x = (rand() % 96) - 48;
            _clientState->xrz.z = (rand() % 96) - 48;
            _respawnTimer = _respawnTime;
            _state = 0x0;
        }
    }

    bool dead() const
    {
        return _health == 0;
    }

    uint8_t state() const
    {
        return _state;
    }

    uint8_t actionId() const
    {
        return _clientInput->clientData & 0xF;
    }

    uint8_t wasd() const
    {
        return (_clientInput->control >> 4) & 0xF;
    }

    float elevation() const
    {
        return _clientInput->elevation;
    }

    uint8_t characterId() const
    {
        return _characterId; // (_clientInput->clientData >> 4) & 0xF;
    }

    void setCharacterId(uint8_t characterId)
    {
        _characterId = characterId;
        _boundingBox.setRadii(4.0f);
    }

    CircleBB* boundingBox()
    {
        return &_boundingBox;
    }

private:
    uint8_t _id;
    CircleBB _boundingBox;
    letsgetsocial::ClientInput* _clientInput;
    letsgetsocial::ClientState* _clientState;
    SOCKADDR_IN _endpoint;
    std::chrono::steady_clock::time_point _lastUpdate;
    std::vector<ScheduledTask> _scheduledTasks;
    uint8_t _health{2};
    uint8_t _state{0};
    uint8_t _characterId{0};
    bool _moveable{true};
    static constexpr float _respawnTime{5.0f};
    float _respawnTimer{_respawnTime};
};