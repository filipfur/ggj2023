#define _USE_MATH_DEFINES

#include <cstdlib>
#include <cmath>

#include "ocean.h"
#include "assetfactory.h"
#include "options.h"
#include "cartoonshading.h"

Ocean::Ocean(BasePipeline *pipeline)
{
    _oceanObject = AssetFactory::getObjects()->ocean->clone();
    _oceanObject->setPosition(0.0, oceanYBaseOffset, 0.0);
    _oceanObject->setScale(glm::vec3(oceanSideLengthX / oceanMeshSideLength, 1.0, oceanSideLengthZ / oceanMeshSideLength));
    //oceanObject->setOpacity()
    dynamic_cast<CartoonShading*>(pipeline)->setOcean(_oceanObject);
}

void Ocean::update(float dt)
{
    lithium::Updateable::update(dt);

    float yOffset = oceanYBaseOffset + tideAmplitude * glm::pow((1 - cos(time() / tidePeriod * 2 * M_PI))/2, tideExponent);
    _oceanObject->setPosition(0.0, yOffset, 0.0);
}
