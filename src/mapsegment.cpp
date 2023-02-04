#define _USE_MATH_DEFINES

#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <cassert>
#include <iostream>

#include "mapsegment.h"
#include "options.h"

MapSegment::MapSegment(std::vector<glm::vec3>& vertexPositions)
{
    // Initialize the height map to all invalid values
    for (int z_idx = 0; z_idx < heightMapResolution; z_idx++) {
        for (int x_idx = 0; x_idx < heightMapResolution; x_idx++) {
            _heights[z_idx][x_idx] = std::nanf("0");
        }
    }

    // Populate the height map with valid numbers
    for(glm::vec3 v : vertexPositions)
    {
        int x_idx = glm::floor((v.x / tileSideLength + 0.5) * numMeshSubdivisionsPerTileSide + 0.5);
        int z_idx = glm::floor((v.z / tileSideLength + 0.5) * numMeshSubdivisionsPerTileSide + 0.5);
        _heights[z_idx][x_idx] = v.y;
    }

    // Check that all values are valid
    for (int z_idx = 0; z_idx < heightMapResolution; z_idx++) {
        for (int x_idx = 0; x_idx < heightMapResolution; x_idx++) {
            if (isnan(_heights[z_idx][x_idx])) {
                // Fill in unfilled heights
                float height_sum = 0.0;
                float weight_sum = 0.0;
                for (int z_prim = z_idx - 1; z_prim <= z_idx + 1; z_prim++) {
                    for (int x_prim = x_idx - 1; x_prim <= x_idx + 1; x_prim++) {
                        if (x_prim >= 0 && x_prim < heightMapResolution &&
                            z_prim >= 0 && z_prim < heightMapResolution )
                        {
                            if (isnan(_heights[z_prim][x_prim])) {
                                continue;
                            }
                            float weight = 1.0;
                            height_sum += weight * _heights[z_prim][x_prim];
                            weight_sum += weight;
                        }
                    }
                }
                if (weight_sum == 0.0) {
                    std::cerr << "Couldn't fill in missing height values for height map" << std::endl;
                    exit(1);
                }
                else {
                    _heights[z_idx][x_idx] = height_sum / weight_sum;
                }
            }
            assert(!isnan(_heights[z_idx][x_idx]));
        }
    }
}

float MapSegment::getHeight(float x, float z) const
{
    int x_idx = std::clamp(static_cast<int>(x / tileSideLength * numMeshSubdivisionsPerTileSide + 0.5), 0, static_cast<int>(numMeshSubdivisionsPerTileSide - 1));
    int z_idx = std::clamp(static_cast<int>(z / tileSideLength * numMeshSubdivisionsPerTileSide + 0.5), 0, static_cast<int>(numMeshSubdivisionsPerTileSide - 1));

    x = x / meshSubdivisionsSideLength - x_idx;
    z = z / meshSubdivisionsSideLength - z_idx;

    return (
        _heights[z_idx    ][x_idx    ] * (1 - z) * (1 - x) +
        _heights[z_idx    ][x_idx + 1] * (1 - z) *      x  +
        _heights[z_idx + 1][x_idx    ] *      z  * (1 - x) +
        _heights[z_idx + 1][x_idx + 1] *      z  *      x  );
}