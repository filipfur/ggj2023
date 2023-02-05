#pragma once

#ifdef _WIN32
#include <winsock2.h>
#else
//#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <stdio.h>
#define SOCKET_ERROR -1
#define SOCKADDR_IN struct sockaddr_in
#define INVALID_SOCKET -1
#define WSAGetLastError() errno
#define WSAEWOULDBLOCK EWOULDBLOCK
#define WSAECONNRESET ECONNRESET
#define ioctlsocket ioctl
#define sockaddr struct sockaddr
#endif
#include <iostream>
#include <iomanip>
#include <sstream>
#include "utility.h"

#include <glm/glm.hpp>

#include "options.h"

namespace letsgetsocial
{

    class ProjectileData
    {
    public:
        ProjectileData(int data) : _data{data}, _clientId{(uint8_t)((data >> 28) & 0xF)},
            _instanceType{(uint8_t)((data >> 24) & 0xF)}, _instanceId{(uint8_t)((data >> 16) & 0xFF)},
            _force{(int)(data >> 12 & 0xF) / 15.0f}, _direction{(int)(data & 0xFFF) / 4095.0f * 360.0f}, _position{0.0f},
            _directionVector{glm::sin(glm::radians(_direction)), 0.0f, glm::cos(glm::radians(_direction))}
        {
        }

        ProjectileData(uint8_t clientId, uint8_t instanceType, uint8_t instanceId, float force, float direction) : _clientId{clientId}, _instanceType{instanceType},
            _instanceId{instanceId}, _force{force}, _direction{direction},
            _data{clientId << 28 | instanceType << 24 | instanceId << 16 | ((int)(force * 15) & 0xF) << 12 | ((int)(direction / 360 * 4095) & 0xFFF)},
            _directionVector{glm::sin(glm::radians(_direction)), 0.0f, glm::cos(glm::radians(_direction))}
        {
        }

        void setPosition(const glm::vec3 position)
        {
            _position = position;
        }

        void setSpeed(float speed)
        {

        }

        void update(float dt)
        {
            _position += _directionVector * dt * 128.0f;
            _spawnTime += dt;
        }

        int data() const
        {
            return _data;
        }

        uint8_t clientId() const
        {
            return _clientId;
        }

        uint8_t instanceType() const
        {
            return _instanceType;
        }

        uint8_t instanceId() const
        {
            return _instanceId;
        }

        uint8_t instanceData() const
        {
            return (uint8_t)((_instanceType << 4) & 0xF0) | (uint8_t)((int)(_force * 15) & 0xF);
        }

        float force() const
        {
            return _force;
        }

        bool olderThan(float seconds)
        {
            return _spawnTime > seconds;
        }

        float direction() const
        {
            return _direction;
        }

        glm::vec3 position() const
        {
            return _position;
        }

        void expire()
        {
            _spawnTime = 9999999.0f;
        }

        float speed() const
        {
            return _speed;
        }

        float damage() const 
        {
            return _damage;
        }

    private:
        int _data;
        uint8_t _clientId;
        uint8_t _instanceType;
        uint8_t _instanceId;
        float _force;
        float _direction;
        glm::vec3 _position{0.0f};
        glm::vec3 _directionVector;
        float _spawnTime{0.0f};
        float _speed{0.0f};
        float _damage{0.0f};
    };

    static constexpr int MAX_CLIENTS = 10;

    class NodeInformation
    {
    public:
        NodeInformation()
        {

        }

        NodeInformation(SOCKADDR_IN sIn) : _sIn{sIn}
        {

        }

        void set(SOCKADDR_IN sIn)
        {
            _sIn = sIn;
        }

        SOCKADDR_IN endpoint() const
        {
            return _sIn;
        }

        std::string toString() const
        {
            std::stringstream ss{};
#ifdef _WIN32
            ss << static_cast<int>(_sIn.sin_addr.S_un.S_un_b.s_b1) << '.' << static_cast<int>(_sIn.sin_addr.S_un.S_un_b.s_b2) << '.'
               << static_cast<int>(_sIn.sin_addr.S_un.S_un_b.s_b3) << '.' << static_cast<int>(_sIn.sin_addr.S_un.S_un_b.s_b4) << ':'
#else
            ss << inet_ntoa(_sIn.sin_addr) << ":"
#endif
               << static_cast<unsigned short>(_sIn.sin_port);
            return ss.str();
        }

    private:
        SOCKADDR_IN _sIn;
    };


    constexpr uint8_t MESSAGE_CLIENT_INPUT{1};
    constexpr uint8_t MESSAGE_CLIENT_STATE{2};
    constexpr uint8_t MESSAGE_CLIENT_IDENTITY{3};
    constexpr uint8_t MESSAGE_CLIENT_MESSAGE{4};

    constexpr uint8_t CLIENT_ACTION_NONE{0};
    constexpr uint8_t CLIENT_ACTION_SHOOTING{1};

    constexpr uint8_t ACTION_KEY_NONE{0x0};
    constexpr uint8_t ACTION_KEY_SPACE{0x1};
    constexpr uint8_t ACTION_KEY_LEFT_MOUSE{0x2};
    constexpr uint8_t ACTION_KEY_Q{0x3};
    constexpr uint8_t ACTION_KEY_E{0x4};
    constexpr uint8_t ACTION_KEY_F{0x5};
    constexpr uint8_t ACTION_KEY_G{0x6};
    constexpr uint8_t ACTION_KEY_1{0xA};
    constexpr uint8_t ACTION_KEY_2{0xB};
    constexpr uint8_t ACTION_KEY_3{0xC};

    struct ClientInput // TODO: Compress e.g. glm::vec3 is uneccesarry
    {
        uint8_t messageType{MESSAGE_CLIENT_INPUT};
        uint8_t clientData; // [CharacterId : ActionId]
        uint8_t control; // W A S D | force;
        float aimDirection;
        float elevation;
    };

    struct ClientState
    {
        glm::vec3 xrz; // X Position, Y Rotation (degrees), Z Position
        int clientData; // [characterId : actionId | clientHealth | force : directionHi | directionLo ]
    };

    enum ClientActionState {
        IDLE = 0,
        HEAD_BUTT = 1,
        DIG = 2,
        AIRBORNE = 3,
        DEATH = 15,
    };

    static constexpr int MAX_PROJECTILES{80};

    struct ClientStateList
    {
        uint8_t messageType{MESSAGE_CLIENT_STATE};
        uint8_t extra0;
        uint8_t extra1;
        uint8_t extra2;
        //int events[MAX_PROJECTILES]; // [ ClientId : InstanceType | InstanceId | Force : DirectionHi | DirectionL ]
        uint8_t resources[goptions::numWorldTilesZ][goptions::numWorldTilesX];
        float time;
        ClientState clientStates[letsgetsocial::MAX_CLIENTS];
    };

    struct ClientIdentity
    {
        uint8_t messageType{MESSAGE_CLIENT_IDENTITY};
        uint8_t clientId;
        uint8_t characterId;
    };

    struct ClientMessage
    {
        uint8_t messageType{MESSAGE_CLIENT_MESSAGE};
        uint8_t messageId;
        char argv[400];
    };

    class Socket
    {
    public:
        	Socket(int portNumber, const std::string inetAddr="127.0.0.1") : _socket{socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)}, _portNumber{portNumber}, _inetAddr{inetAddr}
            {
                if(_socket == INVALID_SOCKET)
                {
                    std::cerr << "failed to create socket: " << WSAGetLastError() << std::endl;
                }
                SOCKADDR_IN localAddress;
#ifndef _WIN32
                memset((char*)&localAddress, 0, sizeof(localAddress));
#endif
                localAddress.sin_family = AF_INET;
                localAddress.sin_port = htons( portNumber );
                localAddress.sin_addr.s_addr = INADDR_ANY;
                if(bind(_socket, (sockaddr*)&localAddress, sizeof( localAddress ) ) == SOCKET_ERROR)
                {
                    std::cerr << "failed to bind socket: " << WSAGetLastError() << std::endl;
                }
            }

            virtual ~Socket()
            {
#ifdef _WIN32
                closesocket(_socket);
#endif
            }

            void setNonBlockingReceive()
            {
                u_long enabled = 1;
                ioctlsocket( _socket, FIONBIO, &enabled );
            }

            bool receive()
            {
                int flags = 0;
                bool rval{true};
                SOCKADDR_IN from;
#ifdef _WIN32
                int fromSize = sizeof(from);
#else
                socklen_t fromSize = sizeof(from);
#endif

                int bytesReceived = recvfrom(_socket, _buffer, SOCKET_BUFFER_SIZE, flags, (sockaddr*)&from, &fromSize);
                if(bytesReceived == SOCKET_ERROR)
                {
                    int error = WSAGetLastError();
                    if(error != WSAEWOULDBLOCK && error != WSAECONNRESET)
                    {
                        std::cerr << "failed to receive with socket: " << error << std::endl;
                        _probablyDead = true;
                    }
                    rval = false;
                }
                else
                {
                    _buffer[bytesReceived] = 0;
                    _lastNodeInformation.set(from);
                    bool print{false};
                    if(print)
                    {
                        std::cout << "======================================================" << std::endl;
                        std::cout << "Received from: " << _lastNodeInformation.toString() << " (bytes: " << bytesReceived << ")" << std::endl;
                        std::cout << _buffer << std::endl;
                        std::cout << utility::hexStr(_buffer, bytesReceived) << std::endl;
                        std::cout << "======================================================" << std::endl << std::endl;
                    }
                }
                return rval;
            }

            bool parse(ClientInput* clientInput)
            {
                bool rval{false};
                if(_buffer[0] == MESSAGE_CLIENT_INPUT)
                {
                    memcpy(clientInput, _buffer, sizeof(ClientInput));
                    rval = true;
                }
                return rval;
            }

            bool parse(ClientStateList* clientStateList)
            {
                static constexpr int size{sizeof(ClientStateList)}; // compute at compiletime
                bool rval{false};
                if(_buffer[0] == MESSAGE_CLIENT_STATE)
                {
                    memcpy(clientStateList, _buffer, size);
                    rval = true;
                }
                return rval;
            }

            bool parse(ClientIdentity* clientIdentity)
            {
                bool rval{false};
                if(_buffer[0] == MESSAGE_CLIENT_IDENTITY)
                {
                    memcpy(clientIdentity, _buffer, sizeof(ClientIdentity));
                    rval = true;
                }
                return rval;
            }

            SOCKADDR_IN endpoint() const
            {
                return _lastNodeInformation.endpoint();
            }

            void send(const std::string& message)
            {
                strncpy(_buffer, message.c_str(), message.length());
                _buffer[message.length()] = 0;
                send(_buffer, message.length() + 1);
            }

            void send(char* buffer, size_t length)
            {
                SOCKADDR_IN serverAddress;
                serverAddress.sin_family = AF_INET;
                serverAddress.sin_port = htons( _portNumber );
#ifdef _WIN32
                serverAddress.sin_addr.S_un.S_addr = inet_addr( _inetAddr.c_str() );
#else
                inet_aton(_inetAddr.c_str(), &serverAddress.sin_addr);
#endif
                send(serverAddress, buffer, length);
            }

            void send(SOCKADDR_IN& endpoint, char* buffer, size_t length)
            {
                int flags{0};
                //std::cout << "sending bytes: " << length << std::endl;
                if(sendto(_socket, buffer, length, flags, (sockaddr*)&endpoint, sizeof(endpoint)) == SOCKET_ERROR)
                {
                    std::cerr << "failed to send with socket: " << WSAGetLastError() << std::endl;
                }
            }

            std::string fromAddress() const
            {
                return _lastNodeInformation.toString();
            }

            bool probablyDead() const
            {
                return _probablyDead;
            }

            void setInetAddr(const std::string& inetAddr)
            {
                _inetAddr = inetAddr;
            }

    private:
#ifdef _WIN32
        SOCKET _socket;
#else
        int _socket;
#endif
        int _portNumber;
        NodeInformation _lastNodeInformation;
        static constexpr int SOCKET_BUFFER_SIZE = 500;
        char _buffer[SOCKET_BUFFER_SIZE];
        bool _probablyDead{false};
        std::string _inetAddr;
    };
};