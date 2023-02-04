#pragma once

#include "basepipeline.h"

class Ocean
{
public:
    Ocean(BasePipeline* pipeline);

    void updateTime(float t);

private:
    lithium::Object* _oceanObject{nullptr};
};
