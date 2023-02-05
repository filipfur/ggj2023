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

        for(int z{0}; z < numWorldTilesZ; ++z)
        {
            for(int x{0}; x < numWorldTilesX; ++x)
            {
                _resources[z][x] = goptions::initialResourceTime;
                _clientStateList.resources[z][x] = 0xFF;
            }   
        }

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
                    _clientStateList.clientStates[nextId].xrz = glm::vec3{(rand() % 16) - 8, 0.0f, (rand() % 16) - 8};

                    std::cout << "New client: " << static_cast<int>(nextId) << std::endl;
                    //_clientStates[nextId].rotation = _clientStates[nextId].speed = _clientStates[nextId].rotationalSpeed = 0.0f;
                    ClientSession clientSession{nextId,
                        _clientInputs[nextId],
                        &_clientStateList.clientStates[nextId],
                        _socket.endpoint()};

                    clientSession.setHealth((uint8_t)128);
                    
                    _clientSessions.emplace(addr, clientSession);
                    newClients.emplace(nextId);
                    if(_clientSessions.size() >= 2)
                    {
                        _gameRunning = true;
                    }
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
            float dt{0.0f};
            //if(_gameRunning)
            //{
                dt = std::chrono::duration<float>(now - _startTick).count();
            //}
            update(dt);
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
        auto it = _scheduledTasks.begin();
        while(it != _scheduledTasks.end())
        {
            if(it->tick(dt))
            {
                it = _scheduledTasks.erase(it);
            }
            else
            {
                ++it;
            }
        }
        float yOffset = oceanYBaseOffset + tideAmplitude * glm::pow((1 - cos(_serverTime / tidePeriod * 2 * 3.14157))/2, tideExponent);
        std::vector<ClientSession*> csVec;
        int alivePlayers{0};
        for(auto&& entry : _clientSessions)
        {
            ClientSession& cs = entry.second;
            csVec.push_back(&cs);

            float lastX = cs.clientState()->xrz.x;
            float lastY = cs.elevation();
            float lastZ = cs.clientState()->xrz.z;
            if(yOffset - lastY > 2.0f)
            {
                cs.setHealth(0);
            }

            if(_gameRunning)
            {
                if(cs.state() == 0x2)
                {
                    float x = lastX;
                    float z = lastZ;
                    x += worldSizeX / 2;
                    z += worldSizeZ / 2;
                    int xIdx = std::clamp(static_cast<int>(x / tileSideLength), 0, static_cast<int>(numWorldTilesX - 1));
                    int zIdx = std::clamp(static_cast<int>(z / tileSideLength), 0, static_cast<int>(numWorldTilesZ - 1));
                    _resources[zIdx][xIdx] -= dt;
                    if(_resources[zIdx][xIdx] <= 0)
                    {
                        _resources[zIdx][xIdx] = 0.0f;
                        cs.setLife(std::max(cs.life() - dt, 0.0f));
                    }
                    else
                    {
                        cs.setLife(std::min(cs.life() + dt * 2.5f, goptions::maxLifeTime));
                    }
                    _clientStateList.resources[zIdx][xIdx] = static_cast<uint8_t>(std::ceilf(_resources[zIdx][xIdx] / goptions::initialResourceTime * 255));
                }
                else
                {
                    cs.setLife(std::max(cs.life() - dt, 0.0f));
                }
                if(cs.state() != 0xF)
                {
                    ++alivePlayers;
                }
            }

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

        // Handle collissions
        for (int idx1 = 0; idx1 < csVec.size(); idx1++) {
            for (int idx2 = idx1 + 1; idx2 < csVec.size(); idx2++) {
                CircleBB& bb1 = *csVec[idx1]->boundingBox();
                CircleBB& bb2 = *csVec[idx2]->boundingBox();
                glm::vec2 n;
                if (!_collisionSystem.test2D(bb1, bb2, n)) {
                    continue;
                }
                // Prevent division by when normalizing
                if (n == glm::vec2{0}) {
                    n = glm::vec2{1.0, 0.0};
                }
                n =  glm::normalize(n);
                float moveDistance = (bb1.radii() + bb2.radii() - glm::distance(glm::vec2{bb1.position().x, bb1.position().z}, glm::vec2{bb2.position().x, bb2.position().z})) / (2);
                csVec[idx1]->clientState()->xrz.x -= moveDistance * n.x;
                csVec[idx1]->clientState()->xrz.z -= moveDistance * n.y;
                csVec[idx2]->clientState()->xrz.x += moveDistance * n.x;
                csVec[idx2]->clientState()->xrz.z += moveDistance * n.y;
            }
        }

        if(_gameRunning)
        {
            _serverTime += dt;
            if(alivePlayers == 1)
            {
                _gameRunning = false;
                _scheduledTasks.push_back(ScheduledTask{5.0f, [this](){
                    for(int z{0}; z < numWorldTilesZ; ++z)
                    {
                        for(int x{0}; x < numWorldTilesX; ++x)
                        {
                            _resources[z][x] = goptions::initialResourceTime;
                            _clientStateList.resources[z][x] = 0xFF;
                        }   
                    }
                    for(auto entry : _clientSessions)
                    {
                        entry.second.setLife(goptions::maxLifeTime * 0.5f);
                        entry.second.clientState()->xrz.x = (rand() % 16) - 8;
                        entry.second.clientState()->xrz.z = (rand() % 16) - 8;
                        entry.second.setState(0x0);
                        entry.second.setHealth(128);
                        _serverTime = 0.1f;
                    }
                    _gameRunning = true;
                }});
            }
        }
        _clientStateList.time = _serverTime;
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
    float _serverTime{0.0f};
    bool _gameRunning{false};
    float _resources[numWorldTilesZ][numWorldTilesX];
    std::vector<ScheduledTask> _scheduledTasks;
};