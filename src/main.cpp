#include "options.h"
#include "lgscore.h"

#include "glapplication.h"
#include "pipeline.h"
#include "assetfactory.h"
#include "world.h"
#include "menu.h"
#include "server.h"
#include "client.h"
#include "potato.h"

void serverThreadFunc(Server* server)
{
    server->serve();
}

class App : public lithium::Application, public IClient
{
public:
    App() : lithium::Application{"GGJ2023 | GG NO RE", glm::ivec2{1600, 900}, lithium::Application::Mode::DEFAULT, false}
    {
        AssetFactory::loadMeshes();
        AssetFactory::loadTextures();
        AssetFactory::loadObjects();
        AssetFactory::loadFonts();
        _characterFactory = new Potato(AssetFactory::getObjects()->potato);
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
                    _client = new Client(this);
                    break;
                case Menu::Action::HostGame:
                    _server = new Server();
                    _serverThread = new std::thread(serverThreadFunc, _server);
                    std::cout << "Started server." << std::endl;
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

    virtual Character* onCharacterSpawned(uint8_t clientId, uint8_t characterId) override
    {
        auto character = _character = createCharacter(clientId, characterId);
        if(_character == nullptr || clientId == _client->clientId())
        {
            _character = character;
        }
        return character;
    }

    virtual void onCharacterDespawned(uint8_t clientId, uint8_t characterId) override
    {
    }

    virtual void onReceivedFromServer(uint8_t clientId, uint8_t characterId, uint8_t health, uint8_t state, float force, float aimDirection, const glm::vec3& xrz) override
    {
        Character* character{nullptr};
        auto it = _characters.find(clientId);
        if(it != _characters.end())
        {
            character = it->second;
            if(character->characterId() != characterId)
            {
                std::vector<lithium::Object*> objects;
                std::vector<lithium::SkinnedObject*> skinnedObjects;
                character->getObjects(objects);
                character->getSkinnedObjects(skinnedObjects);
                std::for_each(objects.begin(), objects.end(), [this](lithium::Object* o){_pipeline->removeObject(o);});
                std::for_each(skinnedObjects.begin(), skinnedObjects.end(), [this](lithium::SkinnedObject* o){_pipeline->removeSkinnedObject(o);});
                if(character == _character)
                {
                    _character = nullptr;
                }
                delete character;
                character = nullptr;
            }
        }
        if(character == nullptr)
        {
            character = onCharacterSpawned(clientId, characterId);
        }
        character->processClientData(
            state,
            health,
            xrz,
            force,
            aimDirection
        );
    }

    Character* createCharacter(uint8_t clientId, uint8_t characterId)
    {
        Character* character{nullptr};

        switch(characterId)
        {
        case 0x1:
            character = _characterFactory->spawn(clientId);
            break;
        }

        std::vector<lithium::Object*> objects;
        character->getObjects(objects);
        _pipeline->insertObjects(objects);

        std::vector<lithium::SkinnedObject*> skinnedObjects;
        character->getSkinnedObjects(skinnedObjects);
        _pipeline->insertSkinnedObjects(skinnedObjects);

        _characters[clientId] = character;
        return character;
    }

    virtual void update(float dt) override
    {
        if(_client)
        {
            _client->update(dt);
        }
        _pipeline->update(dt);
        _pipeline->render();
        _menu->render();
    }
    
private:
    Pipeline* _pipeline{nullptr};
    World* _world{nullptr};
    Menu* _menu{nullptr};
    Client* _client{nullptr};
    Server* _server{nullptr};
    std::thread* _serverThread{nullptr};
    Character* _characterFactory;
    CollisionSystem _collisionSystem;
    std::map<uint8_t, Character*> _characters;
    Character* _character{nullptr};
};

void init()
{
#ifdef _WIN32
    WORD winsock_version = 0x202;
    WSADATA winsock_data;
    if (WSAStartup(winsock_version, &winsock_data))
    {
        std::cerr << "[net] WSAStartup failed: %d\n" << WSAGetLastError() << std::endl;
    }
#endif
}

int main(int argc, const char* argv[])
{
    std::srand(randomSeed);

    init();
    App* app = new App();
    app->run();
    return 0;
}