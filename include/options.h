#pragma once

static constexpr int randomSeed = 0;

static constexpr unsigned int numWorldTilesX = 11;
static constexpr unsigned int numWorldTilesZ = numWorldTilesX;

static constexpr unsigned int numUniqueTiles = 3;

static constexpr float zNear = 0.1;
static constexpr float zFar = 1000.0;

static constexpr float tileSideLength = 30.0;

static constexpr float cameraOffsetX =  0.0 * tileSideLength;
static constexpr float cameraOffsetY =  3.0 * tileSideLength;
static constexpr float cameraOffsetZ = -3.0 * tileSideLength;

static constexpr float mainCharacterStartX = 0.0;
static constexpr float mainCharacterYOffset = 3.0;
static constexpr float mainCharacterStartZ = 0.0;
