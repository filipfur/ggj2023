#include "options.h"
#include "lgscore.h"

#include "glapplication.h"
#include "cartoonshading.h"
#include "assetfactory.h"
#include "world.h"
#include "menu.h"
#include "server.h"
#include "client.h"
#include "potato.h"
#include "ocean.h"

void serverThreadFunc(Server* server)
{
    server->serve();
}

class App : public lithium::Application, public IClient
{
public:
    App() : lithium::Application{"GGJ2023 | GG NO RE", glm::ivec2{1600, 900}, lithium::Application::Mode::MULTISAMPLED_4X, false}
    {
        AssetFactory::loadMeshes();
        AssetFactory::loadTextures();
        AssetFactory::loadObjects();
        AssetFactory::loadFonts();
        _potatoFactory = new Potato(AssetFactory::getObjects()->potato);
        _pipeline = new CartoonShading(defaultFrameBufferResolution());
        
        _zShader = new lithium::ShaderProgram( "shaders/shadowdepth.vert", "shaders/shadowdepth.frag" );
        _zBuffer = new lithium::FrameBuffer(defaultFrameBufferResolution());
        _zBuffer->bind();
            _zBuffer->createTexture(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_NEAREST, GL_CLAMP_TO_BORDER);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            _zBuffer->bindTexture(GL_DEPTH_ATTACHMENT);
            //float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
            //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
            //glBindTexture(GL_TEXTURE_2D, 0);
        _zBuffer->unbind();

        _light = new lithium::Light(AssetFactory::getMeshes()->screen);
        //light->setTexture(bulbTexture);
        _light->setColor(glm::vec4(0.96f, 0.86f, 0.82f, 1.0f));
        _light->setPosition(glm::vec3{8.0f, 64.0f, 8.0f});
        _light->setScale(0.8);
        _pipeline->setLight(_light);
        _world = new World(_pipeline);
        _ocean = new Ocean(_pipeline);
        _menu = new Menu(_pipeline);

        _updateables.push_back(_light);
        _updateables.push_back(_ocean->object());

        input()->addPressedCallback(GLFW_KEY_ESCAPE, [](int mods, int key){
            exit(0);
            return true;
        });

        playAudio("assets/sounds/themesong.wav");

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
            switch(static_cast<Menu::Action>(action))
            {
                case Menu::Action::Connect:
                    _client = new Client(this, input());
                    _client->start();
                    _showMenu = false;
                    break;
                case Menu::Action::HostGame:
                    _server = new Server();
                    //_server->serve();
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
        auto character = createCharacter(clientId, characterId);
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
                //std::vector<lithium::Object*> objects;
                //std::vector<lithium::SkinnedObject*> skinnedObjects;
                //character->getObjects(objects);
                //character->getSkinnedObjects(skinnedObjects);
                //std::for_each(objects.begin(), objects.end(), [this](lithium::Object* o){_pipeline->removeObject(o);});
                //std::for_each(skinnedObjects.begin(), skinnedObjects.end(), [this](lithium::SkinnedObject* o){_pipeline->removeSkinnedObject(o);});
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

        /*switch(characterId)
        {
        case 0x1:
            character = _potatoFactory->spawn(clientId);
            break;
        }*/
        character = _potatoFactory->spawn(clientId);

        std::vector<lithium::Object*> objects;
        std::vector<lithium::SkinnedObject*> skinnedObjects;
        character->getObjects(objects);
        character->getSkinnedObjects(skinnedObjects);

        std::for_each(objects.begin(), objects.end(), [this](lithium::Object* object) {
            _pipeline->addRenderable(object);
        });
        std::for_each(skinnedObjects.begin(), skinnedObjects.end(), [this](lithium::SkinnedObject* skinnedObject) {
            _pipeline->addRenderable(skinnedObject);
        });
        _updateables.insert(_updateables.end(), objects.begin(), objects.end());
        _updateables.insert(_updateables.end(), skinnedObjects.begin(), skinnedObjects.end());

        _characters[clientId] = character;
        return character;
    }

    glm::vec3 mouseToWorldCoordinates() const override
    {
        static glm::vec4 viewport{0.0f, 0.0f, defaultFrameBufferResolution()};
        glm::vec2 mousePos = input()->mousePosition();
        glm::vec3 screenPos = glm::vec3(mousePos.x, viewport.w - mousePos.y, _winZ);
        //glm::mat4 _view = glm::lookAt(_camera->position(), _camera->target(), glm::vec3{0.0f, 1.0f, 0.0f});
        //glm::mat4 modelView = _view;
        return glm::unProject(screenPos, _pipeline->camera()->view(), _pipeline->camera()->projection(), viewport);
    }

    float playerToMouseAngle() const override
    {
        return utility::angle(_character->position(), mouseToWorldCoordinates());
    }

    virtual float playerElevation() const override
    {
        return _character->height();
    }

    virtual void update(float dt) override
    {
        if(_client)
        {
            _client->update(dt);
        }

        if(_client)
        {
            _ocean->updateTime(_client->serverTime());
        }

        glm::mat4 viewProj = _pipeline->camera()->projection() * _pipeline->camera()->view();
        _zShader->setUniform("u_view", viewProj);
        _zBuffer->bind();
        {
            glClear(GL_DEPTH_BUFFER_BIT);
            glActiveTexture(GL_TEXTURE0);
            static auto oc = AssetFactory::getObjects()->ocean->clone();
            oc->setScale(glm::vec3{oceanSideLengthX / oceanMeshSideLength, 1.0, oceanSideLengthZ / oceanMeshSideLength});
            oc->shade(_zShader);
            oc->draw();
            glm::vec2 mp = input()->mousePosition(); // Default frame buffer resolution == LITHIUM_VIEWER_WIDTH ?
            //glReadBuffer(GL_DEPTH_ATTACHMENT);
            glReadPixels( mp.x, defaultFrameBufferResolution().y - mp.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &_winZ );
        }
        _zBuffer->unbind();

        bool allDead{true};
        for(auto it = _characters.begin(); it != _characters.end(); ++it)
        {
            auto character = it->second;
            character->update(dt);
            glm::vec3 p0{character->position()};
            // Update character elevation
            float sourceElevation = character->height();
            float targetElevation = _world->getElevation(p0);
            character->setHeight(sourceElevation + (targetElevation - sourceElevation)*heightDisparityDecaySpeed*dt);

            if(character != _character && character->state() != 0xF)
            {
                allDead = false;
            }

            const glm::ivec2 tId = _world->posToTileId(p0.x, p0.z);
            _world->tileObject(p0.x, p0.z)->setColor(glm::vec4{glm::vec3{(float)_client->resourceValue(tId.x, tId.y) / 255.0f}, 1.0f});

            glm::vec2 normal;
            if(_collisionSystem.checkCollision2D(*_character->boundingBox(), normal))
            {
                //std::cout << "colliding!!" << std::endl;
            }
        }
        _allMyFriendsAreDead = allDead;

        lithium::SimpleCamera* camera = _pipeline->camera();
        if (_character) {
            camera->setTarget(_character->position());
        }
        camera->setPosition(camera->target() + glm::vec3{cameraOffsetX, cameraOffsetY, cameraOffsetZ});

        _pipeline->camera()->update(dt);

        static float near_plane = 1.0f, far_plane = 300.0f;
        static glm::mat4 lightProjection{glm::ortho(-1000.0f, 1000.0f, -1000.0f, 1000.0f, near_plane, far_plane)};
        glm::mat4 lightView{glm::lookAt(_light->position(), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0))};        
        //lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
        auto lightSpaceMatrix = lightProjection * lightView;
        _pipeline->setLightSpaceMatrix(lightSpaceMatrix);
        /*_light->update(dt);
        std::for_each(_objects.begin(), _objects.end(), [dt](lithium::Object* o){
            o->update(dt);
        });
        std::for_each(_skinnedObjects.begin(), _skinnedObjects.end(), [dt](lithium::SkinnedObject* o){
            o->update(dt);
        });
        _ocean->update(dt);*/
        for(auto updateable : _updateables)
        {
            updateable->update(dt);
        }

        _pipeline->render();
        if(_showMenu) { _menu->render(); }
        if(_server != nullptr)
        {
            //_pipeline->textColor(glm::vec3{1.0f, 1.0f, 0.0f});
            //_pipeline->renderText(600.0f, 600.0f, "SERVING");
        }
        if(_character && _character->state() == 0xF)
        {
            _pipeline->textColor(glm::vec3{1.0f, 0.0f, 0.0f});
            _pipeline->textScale(4.0f);
            _pipeline->renderText(150.0f, 400.0f, "GAME OVER");
        }

        if(_client && _client->serverTime() < 0.01f)
        {
            _pipeline->textColor(glm::vec3{1.0f, 1.0f, 1.0f});
            _pipeline->textScale(1.0f);
            _pipeline->renderText(100.0f, 200.0f, "Waiting for other players...");
        }
        if(_character && _client->serverTime() > 0.1f)
        {
            if(_character->state() != 0xF && _allMyFriendsAreDead)
            {
                _pipeline->textColor(glm::vec3{1.0f, 1.0f, 0.0f});
                _pipeline->textScale(4.0f);
                _pipeline->renderText(150.0f, 400.0f, " YOU WON!");
            }
        }
    }
    
private:
    CartoonShading* _pipeline{nullptr};
    lithium::Light* _light{nullptr};
    World* _world{nullptr};
    Ocean* _ocean{nullptr};
    Menu* _menu{nullptr};
    Client* _client{nullptr};
    Server* _server{nullptr};
    std::vector<lithium::Updateable*> _updateables;
    std::thread* _serverThread{nullptr};
    Potato* _potatoFactory{nullptr};
    CollisionSystem _collisionSystem;
    bool _showMenu{true};
    std::map<uint8_t, Character*> _characters; // clientId, Character*
    Character* _character{nullptr};
    float _winZ;
    lithium::ShaderProgram* _zShader{nullptr};
    lithium::FrameBuffer *_zBuffer{nullptr};
    bool _allMyFriendsAreDead{false};
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