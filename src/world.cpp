#include <cstdlib>

#include "world.h"
#include "assetfactory.h"
#include "options.h"

#include "terrainobject.h"

World::World(std::vector<lithium::Updateable*>& updateables, lithium::RenderPipeline *pipeline)
{
    std::shared_ptr<lithium::Object> block = AssetFactory::getObjects()->tiles[0];
    bool hadSpecialTile{false};
    for (int zIdx = 0; zIdx < goptions::numWorldTilesZ; zIdx++) {
        for (int xIdx = 0; xIdx < goptions::numWorldTilesX; xIdx++) {
            int uniqueTileIdx = std::rand() % (hadSpecialTile ? goptions::numUniqueTiles - 1 : goptions::numUniqueTiles);
            if(uniqueTileIdx == 3)
            {
                hadSpecialTile = true;
            }
            uniqueTileIndices[zIdx][xIdx] = uniqueTileIdx;
            float xOffset = (xIdx - float(goptions::numWorldTilesX - 1)/2) * goptions::tileSideLength;
            float zOffset = (zIdx - float(goptions::numWorldTilesZ - 1)/2) * goptions::tileSideLength;

            lithium::Object* tile_clone  = new TerrainObject(*AssetFactory::getObjects()->tiles[uniqueTileIdx]);
            lithium::Object* grass_clone = new lithium::Object(*AssetFactory::getObjects()->grass[uniqueTileIdx]);
            lithium::Object* tree_close  = new lithium::Object(*AssetFactory::getObjects()->trees[uniqueTileIdx]);
            tile_clone->setScale(1.01f);
            tile_clone->setPosition(xOffset, 0.0, zOffset);
            grass_clone->setPosition(xOffset, 0.0, zOffset);
            tree_close->setPosition(xOffset, 0.0, zOffset);
            glm::vec3 rot{0.0f, (rand() % 4) * 90.0f, 0.0f};
            tile_clone ->setRotation(rot);
            grass_clone->setRotation(rot);
            tree_close ->setRotation(rot);
            pipeline->addRenderable(tile_clone);
            pipeline->addRenderable(grass_clone);
            pipeline->addRenderable(tree_close);
            updateables.push_back(tile_clone);
            updateables.push_back(grass_clone);
            updateables.push_back(tree_close);
            if(rand() % 7 == 3)
            {
                auto chestClone = AssetFactory::getObjects()->chest->clone();
                chestClone->setPosition(xOffset, 4.0, zOffset)->setRotation(glm::vec3{0.0f, 180.0f, 0.0f});
                pipeline->addRenderable(chestClone);
                updateables.push_back(chestClone);
            }
            _tileObjects[zIdx][xIdx] = tile_clone;
        }
    }
}

float World::getElevation(float x, float z) const
{
    const glm::ivec2 v = posToTileId(x, z);
    x += goptions::worldSizeX / 2;
    z += goptions::worldSizeZ / 2;
    x -= v.x * goptions::tileSideLength;
    z -= v.y * goptions::tileSideLength;

    return AssetFactory::getMapSegments()[uniqueTileIndices[v.y][v.x]]->getHeight(x, z);
}

float World::getElevation(glm::vec3 v) const
{
    return getElevation(v.x, v.z);
}

void World::forEachTile(const std::function<void(lithium::Object*, int x, int z)>& callback)
{
    for(int z{0}; z < goptions::numWorldTilesZ; ++z)
    {
        for(int x{0}; x < goptions::numWorldTilesX; ++x)
        {
            callback(_tileObjects[z][x], x, z);
        }
    }
}