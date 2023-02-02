#include "glapplication.h"
#include "pipeline.h"
#include "assetfactory.h"

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
    }

    virtual ~App() noexcept
    {

    }

    virtual void update(float dt) override
    {
        _pipeline->update(dt);
        _pipeline->render();
    }
    
private:
    Pipeline* _pipeline{nullptr};
};

int main(int argc, const char* argv[])
{
    App* app = new App();
    app->run();
    return 0;
}