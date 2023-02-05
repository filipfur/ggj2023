#pragma once

namespace goptions
{

    ////////////////////////////////////////////////////////////////////////////////
    // OPTIONS
    ////////////////////////////////////////////////////////////////////////////////

    // Requires constexpr

    static constexpr int randomSeed = 0;

    static constexpr unsigned int numWorldTilesX = 5;
    static constexpr unsigned int numWorldTilesZ = numWorldTilesX;

    static constexpr unsigned int numUniqueTiles = 4;
    static constexpr unsigned int numMeshSubdivisionsPerTileSide = 16;

    // Compile-time intialized in options.cpp

    extern const float zNear;
    extern const float zFar;

    extern const float tileSideLength;

    extern const float oceanMeshSideLength;
    extern const float oceanSideLengthX;
    extern const float oceanSideLengthZ;

    extern const float cameraOffsetX;
    extern const float cameraOffsetY;
    extern const float cameraOffsetZ;

    extern const float oceanYBaseOffset;
    extern const float tideAmplitude;
    extern const float tidePeriod;
    extern const float tideExponent;

    extern const float mainCharacterStartX;
    extern const float mainCharacterYOffset;
    extern const float mainCharacterStartZ;

    extern const float gravity;
    extern const float airborneTime;

    extern const float heightDisparityDecaySpeed;

    extern const float initialResourceTime;
    extern const float maxLifeTime;

    extern const float potatoRadius;
    extern const float headButtSpeed;
    extern const float headButtTime;

    ////////////////////////////////////////////////////////////////////////////////
    // DERIVED CONSTANTS
    ////////////////////////////////////////////////////////////////////////////////

    // Requires constexpr

    static constexpr unsigned int heightMapResolution = numMeshSubdivisionsPerTileSide + 1;

    // Compile-time intialized in options.cpp

    extern const float meshSubdivisionsSideLength;

    extern const float worldSizeX;
    extern const float worldSizeZ;
}
