#pragma once

#include "globject.h"

class TerrainObject : public lithium::Object
{
public:
    TerrainObject(std::shared_ptr<lithium::Mesh> mesh, const std::vector<lithium::Object::TexturePointer>& textures) : lithium::Object{mesh, textures}
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