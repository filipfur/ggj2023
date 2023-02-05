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
    _oceanObject->setPosition(0.0, goptions::oceanYBaseOffset, 0.0);
    _oceanObject->setScale(glm::vec3(goptions::oceanSideLengthX / goptions::oceanMeshSideLength, 1.0, goptions::oceanSideLengthZ / goptions::oceanMeshSideLength));
    //oceanObject->setOpacity()
    dynamic_cast<CartoonShading*>(pipeline)->setOcean(_oceanObject);
}

void Ocean::updateTime(float t)
{
    float yOffset = goptions::oceanYBaseOffset + goptions::tideAmplitude * glm::pow((1 - cos(t / goptions::tidePeriod * 2 * M_PI))/2, goptions::tideExponent);
    _oceanObject->setPosition(0.0, yOffset, 0.0);
}
