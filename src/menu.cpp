#include "menu.h"

Menu::Menu(Pipeline* pipeline) : _pipeline{pipeline}, lithium::Menu("Root Suckers", {new lithium::MenuItem("Play")})
{

}

Menu::~Menu() noexcept
{

}

void Menu::render()
{
    _pipeline->renderText(400.0f, 400.0f, "Testing");
    _pipeline->renderText(500.0f, 500.0f, "Yoo");
}