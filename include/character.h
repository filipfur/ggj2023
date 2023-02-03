#pragma once

#include "glm/glm.hpp"

class Character
{
public:
    Character(glm::vec2 position);

private:
    glm::vec2 _position{0.0, 0.0};
};
