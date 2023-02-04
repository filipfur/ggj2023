#pragma once

#include "options.h"
#include "glrenderpipeline.h"
#include "tile.h"

class World
{
public:
    World(lithium::RenderPipeline* pipeline);

    float getElevation(float x, float z) const;
    float getElevation(glm::vec3 v) const;

private:
    int uniqueTileIndices[numWorldTilesZ][numWorldTilesX];
};
