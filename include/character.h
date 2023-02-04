
#pragma once

#include <glm/glm.hpp>
#include "glupdateable.h"
#include "circlebb.h"
#include "glskinnedobject.h"


class Character : public lithium::Updateable
{
public:
    enum class ActionKey{
        SPACE
    };

    enum class MoveState
    {
        Idle,
        WalkingForward,
        WalkingLeft,
        WalkingBackward,
        WalkingRight
    };

    Character() : _boundingBox{glm::vec3{0.0f}, 3.0f}
    {
        //_boundingBox.setRadii(specification::character(characterId())->circleBBRadii());
    }

    Character(const Character& other) : _boundingBox{other._boundingBox}
    {
    }

    Character* spawn(uint8_t clientId)
    {
        Character* character = clone();
        character->_clientId = clientId;
        return character;
    }

    virtual Character* clone() const = 0;

    virtual ~Character() noexcept
    {
    }

    uint8_t clientId() const
    {
        return _clientId;
    }

    virtual void getObjects(std::vector<lithium::Object*>& objects) = 0;

    virtual void getSkinnedObjects(std::vector<lithium::SkinnedObject*>& skinnedObjects) = 0;

    virtual lithium::Object* object() const = 0;

    virtual uint8_t characterId() const = 0;

    void processClientData(uint8_t state, uint8_t health, const glm::vec3& xrz, float force, float aimDirection)
    {
        glm::vec3 p1{xrz.x, 0.0f, xrz.z};
        glm::vec3 pn{(_targetPosition + p1) / 2.0f};
        glm::vec3 dxyz{p1 - _targetPosition};
        _targetAim = aimDirection;
        _aimDirection = aimDirection;
        _targetPosition = p1;
        if(!_initialized)
        {
            _position = _targetPosition;
            _initialized = true;
        }
        float deltaRotation = xrz.y - _targetDirection;
        _targetDirection = xrz.y;
        glm::vec3 normalizedDelta{glm::normalize(dxyz)};
        float speed = glm::length(dxyz);
        onServerUpdate(state, health, pn, dxyz, xrz.y, _aimDirection, speed, deltaRotation);
    }

    float shortAngleDist(float a0, float a1) {
        float max = 360.0f;//2.0f * 3.141592f;
        float da = glm::mod(a1 - a0, max);
        return glm::mod(2.0f * da, max) - da;
    }

    float distanceBetweenAngles(float targetA, float sourceA)
    {
        float a = targetA - sourceA;
        a += (a>180) ? -360 : (a<-180) ? 360 : 0;
        return a;
    }

    float maxf(float a, float b)
    {
        return a > b ? a : b;
    }

    virtual void update(float dt) override
    {
        _deltaPosition = _targetPosition - _position;
        float distance = sqrt(_deltaPosition.x * _deltaPosition.x + _deltaPosition.z * _deltaPosition.z);

        if(distance > 0.1f)
        {
            distance *= 2.0f;
            float speed = maxf(distance * distance, 8.0f) * dt;
            _position += glm::normalize(_deltaPosition) * speed;
            _moveState = MoveState::WalkingForward;
        }
        else
        {
            _position = _targetPosition;
            _moveState = MoveState::Idle;
        }

        _boundingBox.setPosition(_position);

        _rDistance = distanceBetweenAngles(_targetDirection, _direction);
        //std::cout << rDistance << std::endl;
        if(_rDistance > 0.1f || _rDistance < -0.1f)
        {
            float speed = _rDistance * glm::abs(_rDistance); //glm::max(_rDistance * _rDistance * _rDistance, 90.0f) * dt;
            static const float rSpeedMax = 720.0f;
            static const float rSpeedMin = 8.0f;
            float absSpeed = glm::abs(speed);
            if(absSpeed > rSpeedMax)
            {
                speed = speed / absSpeed * rSpeedMax;
            }
            else if(absSpeed < rSpeedMin)
            {
                speed = speed / absSpeed * rSpeedMin;
            }
            _direction += speed * dt;
        }
        else
        {
            _direction = _targetDirection;
        }
        while(_direction > 180.0f) { _direction -= 360.0f; }
        while(_direction < -180.0f) { _direction += 360.0f; }
    }

    glm::vec3 position() const
    {
        return _position;
    }

    glm::vec3 positionAndHeight() const
    {
        return glm::vec3{_position.x, _height, _position.z};
    }

    float direction() const
    {
        return _direction;
    }

    float targetDirection() const
    {
        return _targetDirection;
    }

    float rDistance() const
    {
        return _rDistance;
    }

    void setTargetPosition(const glm::vec3& targetPosition)
    {
        _targetPosition = targetPosition;
    }

    void setPosition(const glm::vec3& position)
    {
        _position = position;
    }

    void setHeight(float height)
    {
        _height = height;
    }

    float height() const
    {
        return _height;
    }

    CircleBB* boundingBox()
    {
        return &_boundingBox;
    }

protected:

    virtual void onServerUpdate(uint8_t state, uint8_t health, const glm::vec3& position, const glm::vec3& delta,
        float direction, float aimDirection, float speed, float deltaRotation)
    {

    }

    uint8_t _force{0};
    glm::vec3 _deltaPosition{0.0f};
    float _speed{0.0f};
    float _direction{0.0f};
    MoveState _moveState{MoveState::Idle};

private:
    glm::vec3 _targetPosition{0.0f};
    float _targetDirection{0.0f};
    glm::vec3 _position{0.0f};
    float _targetAim{0.0f};
    float _aimDirection{0.0f};
    float _rDistance{0.0f};
    bool _initialized{false};
    float _height{0.0f};
    uint8_t _clientId;
    CircleBB _boundingBox;
};