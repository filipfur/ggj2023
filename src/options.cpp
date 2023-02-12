#include "options.h"


////////////////////////////////////////////////////////////////////////////////
// OPTIONS
////////////////////////////////////////////////////////////////////////////////


const float goptions::zNear = 0.1;
const float goptions::zFar = 1000.0;

const float goptions::tileSideLength = 30.0;

const float goptions::oceanMeshSideLength = 100.0;
const float goptions::oceanSideLengthX = goptions::numWorldTilesX * goptions::tileSideLength;
const float goptions::oceanSideLengthZ = goptions::numWorldTilesZ * goptions::tileSideLength;

const float goptions::cameraOffsetX =  0.0 * goptions::tileSideLength;
const float goptions::cameraOffsetY =  0.75 * goptions::tileSideLength;
const float goptions::cameraOffsetZ = -goptions::cameraOffsetY;

const float goptions::oceanYBaseOffset = 0.3;
const float goptions::tideAmplitude = 3.0;
const float goptions::tidePeriod = 30.0;
const float goptions::tideExponent = 3.0;

const float goptions::mainCharacterStartX = 0.0;
const float goptions::mainCharacterYOffset = 3.0;
const float goptions::mainCharacterStartZ = 0.0;

const float goptions::gravity = 9.82;
const float goptions::airborneTime = 1.0;

const float goptions::heightDisparityDecaySpeed = 16.0;

const float goptions::initialResourceTime = 10.0f;
const float goptions::maxLifeTime = 30.0f;

const float goptions::potatoRadius = 1.0f;
const float goptions::headButtSpeed = 4.0f;
const float goptions::headButtTime = 1.0f;


////////////////////////////////////////////////////////////////////////////////
// DERIVED CONSTANTS
////////////////////////////////////////////////////////////////////////////////


const float goptions::meshSubdivisionsSideLength = goptions::tileSideLength / goptions::numMeshSubdivisionsPerTileSide;

const float goptions::worldSizeX = goptions::numWorldTilesX * goptions::tileSideLength;
const float goptions::worldSizeZ = goptions::numWorldTilesZ * goptions::tileSideLength;
