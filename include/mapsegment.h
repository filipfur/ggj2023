#pragma once

#include <vector>

#include "glm/glm.hpp"
#include "options.h"


class MapSegment
{
public:
    MapSegment(std::vector<glm::vec3>& vertexPositions);

    float getHeight(float x, float z) const;

private:
    float _heights[heightMapResolution][heightMapResolution];
};
