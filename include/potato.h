
#pragma once

#include "character.h"
//#include "glanimatedobject.h"

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

    virtual Potato* clone() const override
    {
        return new Potato(*this);
    }

    virtual ~Potato() noexcept
    {
        delete _potato;
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
        switch(state)
        {
            case 0x0:
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
            case 0x1: // Shoot
                _potato->setAnimation("Push.001");
                break;
            case 0x2: // Root
                _potato->setAnimation("Dig");
                break;
            case 0xF: // Death
                _potato->setAnimation("Dig");
                break;
        }
    }

    virtual void update(float dt) override
    {
        Character::update(dt);
        if(_state == 0x2)
        {
            if(_digDepth > -3.0f)
            {
                _digDepth -= dt * 2.0f;
            }
            else
            {
                _digDepth = -3.0f;
            }
        }
        else
        {
            _digDepth = 0.0f;
        }
        object()->setPosition(positionAndHeight() + glm::vec3{0.0f, _digDepth, 0.0f});
        object()->setRotation(glm::vec3{0.0f, _direction, 0.0f});
    }

    virtual lithium::Object* object() const override
    {
        return _potato;
    }

private:
    lithium::SkinnedObject* _potato;
    float _digDepth{0.0f};
};