#include <cstdlib>

#include "world.h"
#include "assetfactory.h"
#include "options.h"

World::World(lithium::RenderPipeline *pipeline)
{
    lithium::Object* block = AssetFactory::getObjects()->tiles[0];
    bool hadSpecialTile{false};
    for (int zIdx = 0; zIdx < numWorldTilesZ; zIdx++) {
        for (int xIdx = 0; xIdx < numWorldTilesX; xIdx++) {
            int uniqueTileIdx = std::rand() % (hadSpecialTile ? numUniqueTiles - 1 : numUniqueTiles);
            if(uniqueTileIdx == 3)
            {
                hadSpecialTile = true;
            }
            uniqueTileIndices[zIdx][xIdx] = uniqueTileIdx;
            float xOffset = (xIdx - float(numWorldTilesX - 1)/2) * tileSideLength;
            float zOffset = (zIdx - float(numWorldTilesZ - 1)/2) * tileSideLength;

            lithium::Object* tile_clone = AssetFactory::getObjects()->tiles[uniqueTileIdx]->clone();
            lithium::Object* grass_clone = AssetFactory::getObjects()->grass[uniqueTileIdx]->clone();
            lithium::Object* tree_close = AssetFactory::getObjects()->trees[uniqueTileIdx]->clone();
            tile_clone->setPosition(xOffset, 0.0, zOffset);
            grass_clone->setPosition(xOffset, 0.0, zOffset);
            tree_close->setPosition(xOffset, 0.0, zOffset);
            pipeline->insertObject(tile_clone);
            pipeline->insertObject(grass_clone);
            pipeline->insertObject(tree_close);
            _tileObjects[zIdx][xIdx] = tile_clone;
        }
    }
    delete block;
}

float World::getElevation(float x, float z) const
{
    const glm::ivec2 v = posToTileId(x, z);
    x += worldSizeX / 2;
    z += worldSizeZ / 2;
    x -= v.x * tileSideLength;
    z -= v.y * tileSideLength;

    return AssetFactory::getMapSegments()[uniqueTileIndices[v.y][v.x]]->getHeight(x, z);
}

float World::getElevation(glm::vec3 v) const
{
    return getElevation(v.x, v.z);
}

void World::forEachTile(const std::function<void(lithium::Object*, int x, int z)>& callback)
{
    for(int z{0}; z < numWorldTilesZ; ++z)
    {
        for(int x{0}; x < numWorldTilesX; ++x)
        {
            callback(_tileObjects[z][x], x, z);
        }
    }
}