
#pragma once

#include "glmenu.h"
#include "glmenuitem.h"
#include "basepipeline.h"

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

    Menu(BasePipeline* pipeline);
    virtual ~Menu() noexcept;

    void render();

private:
    BasePipeline* _pipeline{nullptr};

};