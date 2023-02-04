#pragma once

#include <glm/glm.hpp>

#include "circlebb.h"
#include "rectanglebb.h"

class CollisionSystem
{
public:

    CollisionSystem()
    {

    }

    virtual ~CollisionSystem() noexcept
    {

    }

    bool checkCollision2D(const CircleBB& circleBB, glm::vec2& normal)
    {
        for(CircleBB& cBB : _circleBBs)
        {
            if(test2D(cBB, circleBB, normal))
            {
                return true;
            }
        }
        for(RectangleBB& rBB : _rectangleBBs)
        {
            if(test2D(rBB, circleBB, normal))
            {
                return true;
            }
        }
        return false;
    }

    bool test2D(const CircleBB& c0, const CircleBB& c1, glm::vec2& n) const
    {
        const glm::vec3 delta{c1.position() - c0.position()};
        //n = glm::normalize(glm::vec2{delta.x, delta.z});
        n = glm::vec2(delta.x, delta.z);
        return sqrt(delta.x * delta.x + delta.z * delta.z) <= (c0.radii() + c1.radii());
    }

    bool test2D(const CircleBB& c0,  const RectangleBB& r1, glm::vec2& n) const
    {
        /*glm::vec3 test{0.0f};
        for(int i{0}; i < 3; ++i)
        {
            if(i == 1) ++i; // Remove for 3D ?
            if(c0.position()[i] < (r1.position()[i] + r1.a()[i]))
            {
                test[i] = r1.position()[i] + r1.a()[i];
            }
            else if(c0.position()[i] > (r1.position()[i] + r1.b()[i]))
            {
                test[i] = r1.position()[i] + r1.b()[i];
            }
        }
        const glm::vec3 delta{c0.position() - test};
        return (delta.x * delta.x + delta.z * delta.z) <= c0.radii2();*/

        // get center point circle first 
        glm::vec2 center(c0.position().x, c0.position().z);
        // calculate AABB info (center, half-extents)
        glm::vec2 aabb_half_extents(r1.dimX() * 0.5f, r1.dimZ() * 0.5f);
        glm::vec2 aabb_center(r1.center().x, r1.center().z);
        // get difference vector between both centers
        glm::vec2 difference = center - aabb_center;
        glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
        // add clamped value to AABB_center and we get the value of box closest to circle
        glm::vec2 closest = aabb_center + clamped;
        // retrieve vector between center circle and closest point AABB and check if length <= radius
        difference = center - closest;
        //n = glm::normalize(difference);
        n = difference;
        return glm::length2(difference) < c0.radii2();
    }

    bool test2D(const RectangleBB& r0, const CircleBB& c1, glm::vec2& n) const
    {
        return test2D(c1, r0, n);
    }

    bool test2D(const RectangleBB& r0, const RectangleBB& r1, glm::vec2& n) const
    {
        // collision x-axis?
        bool collisionX = r0.center().x + r0.dimX() >= r1.center().x &&
            r1.center().x + r1.dimX() >= r0.center().x;
        // collision y-axis?
        bool collisionZ = r0.center().z + r0.dimZ() >= r1.center().z &&
            r1.center().z + r1.dimZ() >= r0.center().z;
        // collision only if on both axes
        return collisionX && collisionZ;
    }


    bool test3D(const CircleBB& c0, const CircleBB& c1)
    {
        return glm::distance(c0.position(), c1.position()) <= (c0.radii() + c1.radii());
    }

    bool test3D(const CircleBB& c0, const RectangleBB& r1) const
    {
        glm::vec3 test{0.0f};
        for(int i{0}; i < 3; ++i)
        {
            if(c0.position()[i] < (r1.position()[i] + r1.a()[i]))
            {
                test[i] = r1.position()[i] + r1.a()[i];
            }
            else if(c0.position()[i] > (r1.position()[i] + r1.b()[i]))
            {
                test[i] = r1.position()[i] + r1.b()[i];
            }
        }

        return glm::distance2(c0.position(), test) <= c0.radii2();
    }

    void insertCircleBB(const CircleBB& circleBB)
    {
        _circleBBs.push_back(circleBB);
    }

    void insertRectangleBB(const RectangleBB& rectangleBB)
    {
        _rectangleBBs.push_back(rectangleBB);
    }

private:
    std::vector<CircleBB> _circleBBs;
    std::vector<RectangleBB> _rectangleBBs;

};