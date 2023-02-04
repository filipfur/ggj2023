#include <cstdlib>

#include "world.h"
#include "assetfactory.h"
#include "options.h"

World::World(lithium::RenderPipeline *pipeline)
{
    lithium::Object* block = AssetFactory::getObjects()->tiles[0];
    for (int zIdx = 0; zIdx < numWorldTilesZ; zIdx++) {
        for (int xIdx = 0; xIdx < numWorldTilesX; xIdx++) {
            int uniqueTileIdx = std::rand() % numUniqueTiles;
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
        }
    }
    delete block;
}

float World::getElevation(float x, float z) const
{
    x += worldSizeX / 2;
    z += worldSizeZ / 2;
    int xIdx = std::clamp(static_cast<int>(x / tileSideLength), 0, static_cast<int>(numWorldTilesX - 1));
    int zIdx = std::clamp(static_cast<int>(z / tileSideLength), 0, static_cast<int>(numWorldTilesZ - 1));
    x -= xIdx * tileSideLength;
    z -= zIdx * tileSideLength;

    return AssetFactory::getMapSegments()[uniqueTileIndices[zIdx][xIdx]]->getHeight(x, z);
}

float World::getElevation(glm::vec3 v) const
{
    return getElevation(v.x, v.z);
}
