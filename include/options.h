#pragma once

static constexpr int randomSeed = 0;

static constexpr unsigned int numWorldTilesX = 11;
static constexpr unsigned int numWorldTilesZ = numWorldTilesX;

static constexpr unsigned int numUniqueTiles = 3;

static constexpr float zNear = 0.1;
static constexpr float zFar = 1000.0;

static constexpr float tileSideLength = 30.0;

static constexpr float oceanMeshSideLength = 100.0;
static constexpr float oceanSideLengthX = numWorldTilesX * tileSideLength;
static constexpr float oceanSideLengthZ = numWorldTilesZ * tileSideLength;

static constexpr float cameraOffsetX =  0.0 * tileSideLength;
static constexpr float cameraOffsetY =  1.0 * tileSideLength;
static constexpr float cameraOffsetZ = -cameraOffsetY;

static constexpr float oceanYBaseOffset = 0.3;
static constexpr float tideAmplitude = 3.0;
static constexpr float tidePeriod = 60.0;
static constexpr float tideExponent = 4.0;

static constexpr float mainCharacterStartX = 0.0;
static constexpr float mainCharacterYOffset = 3.0;
static constexpr float mainCharacterStartZ = 0.0;
