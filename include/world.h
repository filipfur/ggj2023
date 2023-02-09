#pragma once

#include <functional>
#include "options.h"
#include "glrenderpipeline.h"
#include "tile.h"
#include "glupdateable.h"

class World
{
public:
    World(std::vector<lithium::Updateable*>& updateables, lithium::RenderPipeline* pipeline);

    float getElevation(float x, float z) const;
    float getElevation(glm::vec3 v) const;

    glm::ivec2 posToTileId(float x, float z) const
    {
        x += worldSizeX / 2;
        z += worldSizeZ / 2;
        int xIdx = std::clamp(static_cast<int>(x / tileSideLength), 0, static_cast<int>(numWorldTilesX - 1));
        int zIdx = std::clamp(static_cast<int>(z / tileSideLength), 0, static_cast<int>(numWorldTilesZ - 1));
        return glm::ivec2{xIdx, zIdx};
    }

    lithium::Object* tileObject(float x, float z) const
    {
        const glm::ivec2 v = posToTileId(x, z);
        return _tileObjects[v.y][v.x];
    }

    void forEachTile(const std::function<void(lithium::Object*, int x, int z)>& callback);

private:
    int uniqueTileIndices[numWorldTilesZ][numWorldTilesX];
    lithium::Object* _tileObjects[numWorldTilesZ][numWorldTilesX];
};
