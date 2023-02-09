#pragma once

#include "basepipeline.h"

class Ocean
{
public:
    Ocean(BasePipeline* pipeline);

    void updateTime(float t);

    lithium::Object* object() const 
    {
        return _oceanObject;
    }

private:
    lithium::Object* _oceanObject{nullptr};
};
