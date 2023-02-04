#include <cstdlib>

#include "world.h"
#include "assetfactory.h"
#include "options.h"

World::World(lithium::RenderPipeline *pipeline)
{
    lithium::Object* block = AssetFactory::getObjects()->tiles[0];
    for (int tileZ = 0; tileZ < numWorldTilesZ; tileZ++) {
        for (int tileX = 0; tileX < numWorldTilesX; tileX++) {
            //int idx = std::rand() % numUniqueTiles;
            int idx = (tileX + tileZ) % numUniqueTiles;
            float xOffset = (tileX - float(numWorldTilesX - 1)/2) * tileSideLength;
            float zOffset = (tileZ - float(numWorldTilesZ - 1)/2) * tileSideLength;

            lithium::Object* tile_clone = AssetFactory::getObjects()->tiles[idx]->clone();
            lithium::Object* grass_clone = AssetFactory::getObjects()->grass[idx]->clone();
            lithium::Object* tree_close = AssetFactory::getObjects()->trees[idx]->clone();
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
