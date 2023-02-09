#pragma once

#include "globject.h"

class TerrainObject : public lithium::Object
{
public:
    TerrainObject(lithium::Mesh* mesh, lithium::ImageTexture* diffuseTexture) : lithium::Object{mesh, diffuseTexture}
    {

    }

    TerrainObject(const TerrainObject& other) : lithium::Object{other}
    {

    }

    TerrainObject(const Object& other) : lithium::Object{other}
    {

    }

    TerrainObject* clone()
    {
        return new TerrainObject(*this);
    }



private:
};