#pragma once

#include "abstractbb.h"

class CircleBB : public AbstractBB
{
public:
    CircleBB(const glm::vec3& position, float radii) : AbstractBB{position}, _radii{radii}, _radii2{radii * radii}
    {

    }

    CircleBB(const CircleBB& other) : AbstractBB{other}, _radii{other._radii}, _radii2{other._radii2}
    {

    }

    virtual ~CircleBB() noexcept
    {

    }

    void setRadii(float radii)
    {
        _radii = radii;
        _radii2 = radii * radii;
    }

    float radii2() const
    {
        return _radii2;
    }

    float radii() const
    {
        return _radii;
    }

private:
    float _radii;
    float _radii2;
};