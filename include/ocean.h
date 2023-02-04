#pragma once

#include "basepipeline.h"

class Ocean : public lithium::Updateable
{
public:
    Ocean(BasePipeline* pipeline);

    virtual void update(float dt) override;

private:
    lithium::Object* _oceanObject{nullptr};
};
