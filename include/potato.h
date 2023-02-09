
#pragma once

#include "character.h"
//#include "glanimatedobject.h"
#include <iostream>

class Potato : public Character
{
public:

    Potato(lithium::SkinnedObject* potato) 
        : _potato{potato}
    {
        //_potato->animation("shooting")->setLooping(false);
        _potato->animation("Dig")->setLooping(false);
        //_potato->setAnimation("WalkingCycle");
    }

    Potato(const Potato& other) : Character{other}, _potato{other._potato->clone()}
    {
        _potato->setAnimation("WalkingCycle");
    }

    virtual ~Potato() noexcept
    {
        delete _potato;
    }

    virtual Potato* clone() const override
    {
        return new Potato(*this);
    }

    virtual uint8_t characterId() const override { return 0x1; }

    virtual void getObjects(std::vector<lithium::Object*>& objects) override
    {
        //objects.insert(objects.end(), {_potato});
    }

    virtual void getSkinnedObjects(std::vector<lithium::SkinnedObject*>& skinnedObjects) override
    {
        skinnedObjects.insert(skinnedObjects.end(), {_potato});
    }

    virtual void onServerUpdate(uint8_t state, uint8_t health, const glm::vec3& position, const glm::vec3& delta,
        float direction, float aimDirection, float speed, float deltaRotation) override
    {
        Character::onServerUpdate(state, health, position, delta, direction, aimDirection, speed, deltaRotation);
        _pluggSize = (float)health / 255.0f * 2.0f + 0.1f;
        switch(state)
        {
            case letsgetsocial::ClientActionState::IDLE:
                switch(_moveState)
                {
                    case MoveState::Idle:
                        _potato->setAnimation("Idle");
                        break;
                    case MoveState::WalkingForward:
                        _potato->setAnimation("WalkingCycle");
                        break;
                    case MoveState::WalkingLeft:
                        _potato->setAnimation("turningL");
                        break;
                    case MoveState::WalkingBackward:
                        _potato->setAnimation("WalkingCycle");
                        break;
                    case MoveState::WalkingRight:
                        _potato->setAnimation("turningR");
                        break;
                }
                break;
            case letsgetsocial::ClientActionState::HEAD_BUTT: // Shoot
                _potato->setAnimation("Push.001");
                break;
            case letsgetsocial::ClientActionState::DIG: // Root
                _potato->setAnimation("Dig");
                break;
            case letsgetsocial::ClientActionState::AIRBORNE: // Root
                _potato->setAnimation("Dig");
                break;
            case letsgetsocial::ClientActionState::DEATH: // Death
                _potato->setAnimation("Dig");
                break;
        }
    }

    virtual void update(float dt) override
    {
        Character::update(dt);
        if(_state == letsgetsocial::ClientActionState::DIG)
        {
            if(_actionYOffset > -3.0f)
            {
                _actionYOffset -= dt * 2.0f;
            }
            else
            {
                _actionYOffset = -3.0f;
            }
        }
        else
        {
            _actionYOffset = 0.0f;
        }
        object()->setPosition(positionAndHeight() + glm::vec3{0.0f, _actionYOffset, 0.0f});
        object()->setRotation(glm::vec3{0.0f, _direction, 0.0f});
        //float ds = _pluggSize - object->scale().x;
        //object()->setScale(object->scale().x * dt + 2.0f);
        object()->setScale(_pluggSize);
    }

    virtual lithium::Object* object() const override
    {
        return _potato;
    }

private:
    lithium::SkinnedObject* _potato;
    float _actionYOffset{0.0f};
    float _pluggSize;
};