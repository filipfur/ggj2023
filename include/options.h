#pragma once

////////////////////////////////////////////////////////////////////////////////
// OPTIONS
////////////////////////////////////////////////////////////////////////////////

static constexpr int randomSeed = 0;

static constexpr unsigned int numWorldTilesX = 5;
static constexpr unsigned int numWorldTilesZ = numWorldTilesX;

static constexpr unsigned int numUniqueTiles = 4;

static constexpr float zNear = 0.1;
static constexpr float zFar = 1000.0;

static constexpr float tileSideLength = 30.0;
static constexpr unsigned int numMeshSubdivisionsPerTileSide = 16;

static constexpr float oceanMeshSideLength = 100.0;
static constexpr float oceanSideLengthX = numWorldTilesX * tileSideLength;
static constexpr float oceanSideLengthZ = numWorldTilesZ * tileSideLength;

static constexpr float cameraOffsetX =  0.0 * tileSideLength;
static constexpr float cameraOffsetY =  1.0 * tileSideLength;
static constexpr float cameraOffsetZ = -cameraOffsetY;

static constexpr float oceanYBaseOffset = 0.3;
static constexpr float tideAmplitude = 3.0;
static constexpr float tidePeriod = 30.0;
static constexpr float tideExponent = 3.0;

static constexpr float mainCharacterStartX = 0.0;
static constexpr float mainCharacterYOffset = 3.0;
static constexpr float mainCharacterStartZ = 0.0;

static constexpr float heightDisparityDecaySpeed = 16.0;

namespace goptions
{
    extern const float gravity;
    extern const float airborneTime;

    extern const float initialResourceTime;
    extern const float maxLifeTime;

    extern const float headButtSpeed;
    extern const float headButtTime;
}

////////////////////////////////////////////////////////////////////////////////
// DERIVED CONSTANTS
////////////////////////////////////////////////////////////////////////////////

static constexpr unsigned int heightMapResolution = numMeshSubdivisionsPerTileSide + 1;
static constexpr float meshSubdivisionsSideLength = tileSideLength / numMeshSubdivisionsPerTileSide;

static const float worldSizeX = numWorldTilesX * tileSideLength;
static const float worldSizeZ = numWorldTilesZ * tileSideLength;