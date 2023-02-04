#pragma once

#include "pipeline.h"

class Ocean
{
public:
    Ocean(Pipeline* pipeline);

private:
    lithium::Object* oceanObject{nullptr};
};
