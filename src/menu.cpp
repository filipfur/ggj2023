#include "menu.h"

Menu::Menu(BasePipeline* pipeline) : _pipeline{pipeline}, lithium::Menu("Root Suckers", {
    (new lithium::MenuItem("Connect"))->setActionId(static_cast<int>(Action::Connect)),
    (new lithium::MenuItem("Host game (server)"))->setActionId(static_cast<int>(Action::HostGame)),
    (new lithium::MenuItem("Options"))->setActionId(static_cast<int>(Action::Options)),
    (new lithium::MenuItem("Exit"))->setActionId(static_cast<int>(Action::Exit))
    })
{

}

Menu::~Menu() noexcept
{

}

void Menu::render()
{
    /*_pipeline->renderText(400.0f, 400.0f, "Testing");
    _pipeline->renderText(500.0f, 500.0f, "Yoo");*/
    _pipeline->renderText(800.0f, 760.0f, "Tater Tangle", 6.0f);
    int i{0};
    forEachChild([this, &i](lithium::MenuItem* menuItem){
        bool isCurrent{menuItem == this->current()};
        _pipeline->renderText(800.0f, 500.0f - i * 100.0f, (isCurrent ? "" : "") + menuItem->label(),
            2.0f,
            isCurrent ? glm::vec3{0.5f, 1.0f, 0.5f} : glm::vec3{1.0f, 1.0f, 1.0f}, true, false);
        ++i;
    });
}