#include "world.h"
#include "pipeline.h"
#include "assetfactory.h"
#include "options.h"

World::World(Pipeline *pipeline)
{
    lithium::Object* block = AssetFactory::getObjects()->block;

    for (int tileZ = 0; tileZ < numWorldTilesZ; tileZ++) {
        for (int tileX = 0; tileX < numWorldTilesX; tileX++) {
            lithium::Object* block_clone = block->clone();
            block_clone->setPosition((tileX - float(numWorldTilesX - 1)/2) * tileSideLength, 0.0, (tileZ - float(numWorldTilesZ - 1)/2) * tileSideLength);
            pipeline->insertObject(block_clone);
        }
    }

    delete block;
}
