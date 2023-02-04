#include "menu.h"

Menu::Menu(BasePipeline* pipeline) : _pipeline{pipeline}, lithium::Menu("Root Suckers", {
    (new lithium::MenuItem("Connect"))->setActionId(static_cast<int>(Action::Connect)),
    (new lithium::MenuItem("Host game (local)"))->setActionId(static_cast<int>(Action::HostGame)),
    (new lithium::MenuItem("Options"))->setActionId(static_cast<int>(Action::Options)),
    (new lithium::MenuItem("Exit"))->setActionId(static_cast<int>(Action::Exit))})
{

}

Menu::~Menu() noexcept
{

}

void Menu::render()
{
    /*_pipeline->renderText(400.0f, 400.0f, "Testing");
    _pipeline->renderText(500.0f, 500.0f, "Yoo");*/
    int i{0};
    forEachChild([this, &i](lithium::MenuItem* menuItem){
        if(menuItem == this->current())
        {
            _pipeline->textColor(glm::vec3{1.0f, 1.0f, 1.0f});
        }
        else
        {
            _pipeline->textColor(glm::vec3{0.0f, 1.0f, 0.0f});
        }
        _pipeline->renderText(900.0f, 500.0f + i * 100.0f, menuItem->label());
        ++i;
    });
}