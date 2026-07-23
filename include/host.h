#ifndef HOST_H
#define HOST_H

#include <enet/enet.h>
#include <iostream>
#include "net_packets.h"

class Host {
public:
    Host(uint16_t port = 7777) : _server(nullptr), _clientPeer(nullptr), _isClientConnected(false), _clientPosY(100.0f), _hasClientInput(false) {
        ENetAddress address{};
        address.host = ENET_HOST_ANY;
        address.port = port;

        this->_server = enet_host_create(&address, 2, 2, 0, 0);
        if (this->_server == nullptr) {
            std::cerr << "[Host] Error creating ENet server host on port " << port << std::endl;
        } else {
            std::cout << "[Host] Server initialized on port " << port << std::endl;
        }
    }

    ~Host() {
        this->disconnect();
        if (this->_server) {
            enet_host_destroy(this->_server);
            this->_server = nullptr;
        }
    }

    void update() {
        if (!this->_server) return;

        ENetEvent event;
        while (enet_host_service(this->_server, &event, 0) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    std::cout << "[Host] Client connected from " << event.peer->address.host << ":" << event.peer->address.port << std::endl;
                    this->_clientPeer = event.peer;
                    this->_isClientConnected = true;
                    this->broadcastLobbyStatus(false);
                    break;
                case ENET_EVENT_TYPE_RECEIVE:
                    if (event.packet->dataLength == sizeof(ClientInputPacket)) {
                        ClientInputPacket* pkt = reinterpret_cast<ClientInputPacket*>(event.packet->data);
                        if (pkt->type == PACKET_CLIENT_INPUT) {
                            this->_clientPosY = pkt->clientPosY;
                            this->_hasClientInput = true;
                        }
                    }
                    enet_packet_destroy(event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    std::cout << "[Host] Client disconnected." << std::endl;
                    this->_clientPeer = nullptr;
                    this->_isClientConnected = false;
                    break;
                default:
                    break;
            }
        }
    }

    void broadcastGameState(const HostGameStatePacket& state) {
        if (!this->_clientPeer || !this->_isClientConnected) return;

        ENetPacket* packet = enet_packet_create(&state, sizeof(HostGameStatePacket), 0); // Unreliable for high frequency updates
        enet_peer_send(this->_clientPeer, 0, packet);
    }

    void broadcastLobbyStatus(bool gameStarted) {
        if (!this->_clientPeer || !this->_isClientConnected) return;

        LobbyStatusPacket pkt;
        pkt.type = PACKET_LOBBY_STATUS;
        pkt.clientConnected = this->_isClientConnected ? 1 : 0;
        pkt.gameStarted = gameStarted ? 1 : 0;

        ENetPacket* packet = enet_packet_create(&pkt, sizeof(LobbyStatusPacket), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(this->_clientPeer, 0, packet);
    }

    void disconnect() {
        if (this->_clientPeer && this->_isClientConnected) {
            enet_peer_disconnect(this->_clientPeer, 0);
            ENetEvent event;
            while (enet_host_service(this->_server, &event, 100) > 0) {
                if (event.type == ENET_EVENT_TYPE_DISCONNECT) break;
            }
            this->_clientPeer = nullptr;
            this->_isClientConnected = false;
        }
    }

    inline bool isClientConnected() const { return this->_isClientConnected; }
    inline float getClientPosY() const { return this->_clientPosY; }
    inline bool hasClientInput() const { return this->_hasClientInput; }

private:
    ENetHost* _server;
    ENetPeer* _clientPeer;
    bool _isClientConnected;
    float _clientPosY;
    bool _hasClientInput;
};

#endif /* HOST_H */