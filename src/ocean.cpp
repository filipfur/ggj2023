#include <cstdlib>

#include "ocean.h"
#include "assetfactory.h"
#include "options.h"
#include "cartoonshading.h"

Ocean::Ocean(BasePipeline *pipeline)
{
    oceanObject = AssetFactory::getObjects()->ocean->clone();
    oceanObject->setPosition(0.0, oceanYBaseOffset, 0.0);
    oceanObject->setScale(glm::vec3(oceanSideLengthX / oceanMeshSideLength, 1.0, oceanSideLengthZ / oceanMeshSideLength));
    //oceanObject->setOpacity()
    dynamic_cast<CartoonShading*>(pipeline)->setOcean(oceanObject);
}
