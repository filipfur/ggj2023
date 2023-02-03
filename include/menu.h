
#pragma once

#include "glmenu.h"
#include "glmenuitem.h"
#include "pipeline.h"

class Menu : public lithium::Menu
{
public:

    enum class Action
    {
        Connect,
        HostGame,
        Options,
        Exit
    };

    Menu(Pipeline* pipeline);
    virtual ~Menu() noexcept;

    void render();

private:
    Pipeline* _pipeline{nullptr};

};