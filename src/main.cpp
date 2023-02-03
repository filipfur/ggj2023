#include "glapplication.h"
#include "pipeline.h"
#include "assetfactory.h"
#include "world.h"
#include "menu.h"

class App : public lithium::Application
{
public:
    App() : lithium::Application{"GGJ2023 | GG NO RE", glm::ivec2{1600, 900}, lithium::Application::Mode::DEFAULT, false}
    {
        AssetFactory::loadMeshes();
        AssetFactory::loadTextures();
        AssetFactory::loadObjects();
        AssetFactory::loadFonts();
        _pipeline = new Pipeline(defaultFrameBufferResolution());
        _world = new World(_pipeline);
        _menu = new Menu(_pipeline);
        input()->addPressedCallback(GLFW_KEY_UP, [this](int mods, int key){
            _menu->next();
            return true;
        });
        input()->addPressedCallback(GLFW_KEY_DOWN, [this](int mods, int key){
            _menu->previous();
            return true;
        });
        input()->addPressedCallback(GLFW_KEY_ENTER, [this](int mods, int key){
            int action = _menu->enter();
            std::cout << "YIPPI: " << action << std::endl;
            switch(action)
            {
                case Menu::Action::Connect:
                    break;
                case Menu::Action::HostGame:
                    break;
                case Menu::Action::Options:
                    break;
                case Menu::Action::Exit:
                    exit(0);
                    break;
            }
            return true;
        });
    }

    virtual ~App() noexcept
    {

    }

    virtual void update(float dt) override
    {
        _pipeline->update(dt);
        _pipeline->render();
        _menu->render();
    }
    
private:
    Pipeline* _pipeline{nullptr};
    World* _world{nullptr};
    Menu* _menu{nullptr};
};

int main(int argc, const char* argv[])
{
    App* app = new App();
    app->run();
    return 0;
}