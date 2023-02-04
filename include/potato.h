
#pragma once

#include "character.h"
//#include "glanimatedobject.h"

class Potato : public Character
{
public:

    Potato(lithium::Object* potato) 
        : _potato{potato}
    {
        //_potato->animation("shooting")->setLooping(false);
    }

    Potato(const Potato& other) : Character{other}, _potato{other._potato->clone()}
    {
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
        objects.insert(objects.end(), {_potato});
    }

    virtual void getSkinnedObjects(std::vector<lithium::SkinnedObject*>& skinnedObjects) override
    {
        //skinnedObjects.insert(skinnedObjects.end(), {_potato});
    }

    virtual void onServerUpdate(uint8_t state, uint8_t health, const glm::vec3& position, const glm::vec3& delta,
        float direction, float aimDirection, float speed, float deltaRotation) override
    {
        /*switch(state)
        {
            case 0x0:
                switch(_moveState)
                {
                    case MoveState::Idle:
                        _potato->setAnimation("idle");
                        break;
                    case MoveState::WalkingForward:
                        _potato->setAnimation("walkingForward");
                        break;
                    case MoveState::WalkingLeft:
                        _potato->setAnimation("turningL");
                        break;
                    case MoveState::WalkingBackward:
                        _potato->setAnimation("walkingForward");
                        break;
                    case MoveState::WalkingRight:
                        _potato->setAnimation("turningR");
                        break;
                }
                break;
            case 0x1: // Shoot
                _potato->setAnimation("shooting");
                break;
            case 0xF: // Death
                _potato->setAnimation("death");
                break;
        }*/
    }

    virtual void update(float dt) override
    {
        Character::update(dt);
        object()->setPosition(positionAndHeight());
        object()->setRotation(glm::vec3{0.0f, _direction, 0.0f});
    }

    virtual lithium::Object* object() const override
    {
        return _potato;
    }

private:
    lithium::Object* _potato;
};