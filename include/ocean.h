#pragma once

#include "basepipeline.h"

class Ocean
{
public:
    Ocean(BasePipeline* pipeline);

private:
    lithium::Object* oceanObject{nullptr};
};
