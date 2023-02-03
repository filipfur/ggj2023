
#pragma once

#include "glmenu.h"
#include "glmenuitem.h"
#include "pipeline.h"

class Menu : lithium::Menu
{
public:
    Menu(Pipeline* pipeline);
    virtual ~Menu() noexcept;

    void render();

private:
    Pipeline* _pipeline{nullptr};

};