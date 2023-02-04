#pragma once

#include <iostream>
#include <thread>
#include <chrono>

#include "lgscore.h"

#include "character.h"
#include "collisionsystem.h"
#include "iclient.h"

#include "glinput.h"

class Client
{
public:
    Client(IClient* iClient, lithium::Input* input) : _iClient{iClient}, _socket{25565}
    {
        _keyCache = new lithium::Input::KeyCache({GLFW_KEY_SPACE, GLFW_KEY_Q, GLFW_KEY_E, GLFW_KEY_F, GLFW_KEY_G,
            GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3,
            GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D});
        input->setKeyCache(_keyCache);
    }

    void start()
    {
        _socket.send("Hello world!");

        _socket.receive();
        _endpoint = _socket.endpoint();

        if(_socket.parse(&_clientIdentity))
        {
            std::cout << "My client identity is: " << (int)_clientIdentity.clientId << std::endl;
            _iClient->onCharacterSpawned(_clientIdentity.clientId, _clientIdentity.characterId);
        }
        else
        {
            std::cerr << "Failed to parse ClientIdentity on client side!" << std::endl;
        }

        auto f = [this](){
            for(;;)
            {
                bool receivedFromServer{false};
                {
                    std::lock_guard<std::mutex> lockGuard{_socketMutex};
                    while(_socket.receive())
                    {
                        receivedFromServer = true;
                        if(!_socket.parse(&_clientStatesTemporary))
                        {
                            std::cerr << "Failed to parse ClientStateList on client side!" << std::endl;
                        }
                    }
                }
                if(receivedFromServer)
                {
                    std::lock_guard<std::mutex> lockGuard{_clientStatesMutex};
                    std::memcpy(&_clientStates, &_clientStatesTemporary, _clientStateListSize);
                    _clientStateUpdated = true;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds{5});
            }
        };

        _thread.reset(new std::thread(f));
    }

    void update(float dt)
    {
        bool receivedFromServer{false};
        { // Locked for client-to-server communication. Send client input.
            std::lock_guard<std::mutex> lockGuard(_clientStatesMutex);
            if(_clientStateUpdated)
            {
                std::memcpy(&_clientStatesCopy, &_clientStates, _clientStateListSize);
                receivedFromServer = true;
                _clientStateUpdated = false;
            }
            //letsgetsocial::ClientInput cs{letsgetsocial::MESSAGE_CLIENT_INPUT, controller.delta()};
            //_socket.send(_endpoint, (char*)&cs, sizeof(cs));
        }

        if(receivedFromServer) // only update rates when received new data from server
        {
            for(uint8_t i = 0; i < letsgetsocial::MAX_CLIENTS; ++i)
            {
                letsgetsocial::ClientState* clientState = &_clientStatesCopy.clientStates[i];
                int clientData = clientState->clientData;
                if((clientData & 0xF0000000) == 0)
                {
                    continue;
                }

                uint8_t characterId = (clientData >> 28) & 0xF;

                uint8_t health = (clientData >> 16) & 0xFF;
                uint8_t state = (clientData >> 24) & 0x0F;

                float force = (int)(clientData >> 12 & 0xF) / 15.0f;
                float aimDirection = (int)(clientData & 0xFFF) / 4095.0f * 360.0f;

                _iClient->onReceivedFromServer(i, characterId, health, state, force, aimDirection, clientState->xrz);
            }
        }

        static int sendCounter = 4; // TODO: assesment
        if(sendCounter-- <= 0)
        {
            sendCounter = 4;
            std::lock_guard<std::mutex> lockGuard(_socketMutex);

            //glm::vec3 cursorPos{mouseToWorldCoordinates()};

            uint8_t wasd{0};
            uint8_t actionKey{0};

            if(_keyCache->isPressed(GLFW_KEY_SPACE))
            {
                actionKey = letsgetsocial::ACTION_KEY_SPACE;
            }
            else if(_keyCache->isPressed(GLFW_KEY_Q))
            {
                actionKey = letsgetsocial::ACTION_KEY_Q;
            }
            else if(_keyCache->isPressed(GLFW_KEY_E))
            {
                actionKey = letsgetsocial::ACTION_KEY_E;
            }
            else if(_keyCache->isPressed(GLFW_KEY_F))
            {
                actionKey = letsgetsocial::ACTION_KEY_F;
            }
            else if(_keyCache->isPressed(GLFW_KEY_G))
            {
                actionKey = letsgetsocial::ACTION_KEY_G;
            }
            else if(_keyCache->isPressed(GLFW_KEY_1))
            {
                actionKey = letsgetsocial::ACTION_KEY_1;
            }
            else if(_keyCache->isPressed(GLFW_KEY_2))
            {
                actionKey = letsgetsocial::ACTION_KEY_2;
            }
            else if(_keyCache->isPressed(GLFW_KEY_3))
            {
                actionKey = letsgetsocial::ACTION_KEY_3;
            }


            if(_keyCache->isPressed(GLFW_KEY_W))
            {
                std::cout << "W" << std::endl;
                wasd |= 0b1000;
            }
            if(_keyCache->isPressed(GLFW_KEY_A))
            {
                wasd |= 0b0100;
            }
            if(_keyCache->isPressed(GLFW_KEY_S))
            {
                wasd |= 0b0010;
            }
            if(_keyCache->isPressed(GLFW_KEY_D))
            {
                wasd |= 0b0001;
            }

            float force;
            letsgetsocial::ClientInput clientInput;
            clientInput.clientData = (_clientIdentity.characterId << 4) | actionKey;
            clientInput.control = ((wasd << 4) & 0xF0) | 0/*_character->force()*//*_force*/ & 0xF;
            _socket.send(_endpoint, (char*)&clientInput, sizeof(letsgetsocial::ClientInput));
        }
    }

    uint8_t clientId() const
    {
        return _clientIdentity.clientId;
    }

    private:
        IClient* _iClient;
        std::unique_ptr<std::thread> _thread;
        lithium::Input::KeyCache* _keyCache{nullptr};
        letsgetsocial::Socket _socket;
        letsgetsocial::ClientStateList _clientStates;
        letsgetsocial::ClientStateList _clientStatesTemporary;
        letsgetsocial::ClientStateList _clientStatesCopy;
        bool _clientStateUpdated{false};
        letsgetsocial::ClientIdentity _clientIdentity;
        SOCKADDR_IN _endpoint;
        static constexpr int _clientStateListSize{sizeof(letsgetsocial::ClientStateList)};
        std::mutex _clientStatesMutex;
        std::mutex _socketMutex;
};