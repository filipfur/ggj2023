#include "lgscore.h"

#include <iomanip>
#include <thread>
#include <map>
#include <chrono>
#include <set>
#include <vector>

#include <glm/glm.hpp>

#include "clientsession.h"
#include "collisionsystem.h"
#include "gltfloader.h"
#include "rectanglebb.h"

class Server
{
public:
    Server() : _socket{25563}, _clientInputs{}, _clientStateList{}
    {
        memset(&_clientStateList, 0, sizeof(letsgetsocial::ClientStateList));
        _clientStateList.messageType = letsgetsocial::MESSAGE_CLIENT_STATE;
        _socket.setNonBlockingReceive();

        /*std::vector<lithium::Node*> mapProperties;
        _gltfLoader.loadNodeProperties("assets/map/map.gltf", mapProperties);

        for(auto& node : mapProperties)
        {
            if(node->hasPropertyF("circleBB"))
            {
                _collisionSystem.insertCircleBB(CircleBB{node->position(), node->propertyF("circleBB")});
            }
            if(node->hasPropertyArrayF("rectangleBB"))
            {
                const std::vector<float>& dat = node->propertyArrayF("rectangleBB");
                _collisionSystem.insertRectangleBB(RectangleBB{node->position(),
                    glm::vec3{dat[0], dat[1], dat[2]},
                    glm::vec3{dat[3], dat[4], dat[5]}});
            }
        }*/
    }

    virtual ~Server()
    {
        for(size_t i = 0; i < letsgetsocial::MAX_CLIENTS; ++i)
        {
            delete _clientInputs[i];
        }
    }

    void serve()
    {
        _startServer = std::chrono::steady_clock::now();
        _startTick = std::chrono::steady_clock::now();
        for(;;)
        {
            std::set<int> newClients;
            while(_socket.receive())
            {
                uint8_t nextId = 0xFF;
                const std::string addr{_socket.fromAddress()};
                auto it = _clientSessions.find(addr);
                if(it == _clientSessions.end())
                {
                    nextId = nextFreeClientId();
                    _clientInputs[nextId] = new letsgetsocial::ClientInput();
                    //letsgetsocial::ClientState clientState{letsgetsocial::MESSAGE_CLIENT_STATE, nextId, glm::vec3{0.0f}, 0.0f, 0.0f, 0.0f};
                    _clientStateList.clientStates[nextId].xrz = glm::vec3{(rand() % 96) - 48, 0.0f, (rand() % 96) - 48};

                    std::cout << "New client: " << nextId << std::endl;
                    //_clientStates[nextId].rotation = _clientStates[nextId].speed = _clientStates[nextId].rotationalSpeed = 0.0f;
                    ClientSession clientSession{nextId,
                        _clientInputs[nextId],
                        &_clientStateList.clientStates[nextId],
                        _socket.endpoint()};

                    clientSession.setHealth((uint8_t)2);
                    
                    _clientSessions.emplace(addr, clientSession);
                    newClients.emplace(nextId);
                }
                else
                {
                    if(_socket.parse(it->second.clientInput()))
                    {
                        it->second.refresh();
                    }
                    else
                    {
                        std::cerr << "Failed to parse message on server side!" << std::endl;
                    }
                }
            }
            auto now = std::chrono::steady_clock::now();
            update(std::chrono::duration<float>(now - _startTick).count());
            _startTick = now;
            std::map<std::string, ClientSession>::iterator it = _clientSessions.begin();
            while(it != _clientSessions.end())
            {
                bool shouldRemove{false};
                ClientSession* clientSession = &(it->second);
                if(newClients.find(clientSession->id()) != newClients.end())
                {
                    letsgetsocial::ClientIdentity clientIdentity;
                    clientIdentity.clientId = clientSession->id();
                    clientIdentity.characterId = 0x3 - (uint8_t)(clientSession->id() % 3);
                    clientIdentity.characterId = 0x1;
                    clientSession->setCharacterId(0x1);
                    _socket.send(clientSession->endpoint(), (char*)&clientIdentity, sizeof(letsgetsocial::ClientIdentity));
                    std::cout << "sending client identity." << std::endl;
                }
                else if(std::chrono::duration<float>(now - clientSession->lastUpdate()).count() > 3.0f) // No response in three seconds.
                {
                    int id = static_cast<int>(clientSession->id());
                    shouldRemove = true;
                    delete _clientInputs[id];
                    _clientInputs[id] = nullptr;
                }
                else
                {
                    _socket.send(clientSession->endpoint(), (char*)&_clientStateList, sizeof(letsgetsocial::ClientStateList));
                }
                if(shouldRemove)
                {
                    it = _clientSessions.erase(it);
                }
                else
                {
                    ++it;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds{50});
        }
    }

    void update(float dt)
    {
        for(auto&& entry : _clientSessions)
        {
            ClientSession& cs = entry.second;

            float lastX = cs.clientState()->xrz.x;
            float lastZ = cs.clientState()->xrz.z;
            cs.update(dt);
            auto bb = cs.boundingBox();
            bb->setPosition(glm::vec3{cs.clientState()->xrz.x, 0.0f, cs.clientState()->xrz.z});
            glm::vec2 normal{};
            if(_collisionSystem.checkCollision2D(*bb, normal))
            {
                glm::vec2 bounce{normal};
                float amt = 0.1f;
                if(bounce.x < 0) { bounce.x = -amt; }  else if(bounce.x > 0) {bounce.x = +amt; }
                if(bounce.y < 0) { bounce.y = -amt; }  else if(bounce.y > 0) {bounce.y = +amt; }
                do
                {
                    //cs.clientState()->xrz.x = lastX;
                    //cs.clientState()->xrz.z = lastZ;
                    cs.clientState()->xrz.x += bounce.x;
                    cs.clientState()->xrz.z += bounce.y;
                    bb->setPosition(glm::vec3{cs.clientState()->xrz.x, 0.0f, cs.clientState()->xrz.z});
                } while(_collisionSystem.checkCollision2D(*bb, normal));
            }
            cs.updateClientData();
        }
    }

private:
    uint8_t nextFreeClientId() const
    {
        for(uint8_t i = 0; i < letsgetsocial::MAX_CLIENTS; ++i)
        {
            if(_clientInputs[i] == nullptr)
            {
                return i;
            }
        }
        return (uint8_t)0xFF;
    }

    letsgetsocial::Socket _socket;
    letsgetsocial::ClientInput* _clientInputs[letsgetsocial::MAX_CLIENTS];
    letsgetsocial::ClientStateList _clientStateList;
    std::map<std::string, ClientSession> _clientSessions;
    std::chrono::steady_clock::time_point _startTick;
    std::chrono::steady_clock::time_point _startServer;
    CollisionSystem _collisionSystem;
    gltf::Loader _gltfLoader;
};