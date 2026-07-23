#ifndef CLIENT_H
#define CLIENT_H

#include <enet/enet.h>
#include <iostream>
#include <string>
#include "net_packets.h"

enum class ClientConnectionStatus {
    DISCONNECTED,
    CONNECTING,
    CONNECTED
};

class Client {
public:
    Client() : _client(nullptr), _peer(nullptr), _status(ClientConnectionStatus::DISCONNECTED), _hasNewState(false) {
        this->_client = enet_host_create(NULL, 1, 2, 0, 0);
        if (this->_client == nullptr) {
            std::cerr << "[Client] Error creating ENet client host." << std::endl;
        }
    }

    ~Client() {
        this->disconnect();
        if (this->_client) {
            enet_host_destroy(this->_client);
            this->_client = nullptr;
        }
    }

    bool connectToHost(const char* hostAddress = "127.0.0.1", uint16_t port = 7777) {
        if (!this->_client) return false;
        
        ENetAddress address;
        enet_address_set_host(&address, hostAddress);
        address.port = port;

        this->_peer = enet_host_connect(this->_client, &address, 2, 0);
        if (this->_peer == nullptr) {
            std::cerr << "[Client] No available peer for connection." << std::endl;
            this->_status = ClientConnectionStatus::DISCONNECTED;
            return false;
        }

        this->_status = ClientConnectionStatus::CONNECTING;
        return true;
    }

    void update() {
        if (!this->_client) return;

        ENetEvent event;
        while (enet_host_service(this->_client, &event, 0) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    std::cout << "[Client] Connected to host!" << std::endl;
                    this->_status = ClientConnectionStatus::CONNECTED;
                    break;
                case ENET_EVENT_TYPE_RECEIVE:
                    if (event.packet->dataLength == sizeof(HostGameStatePacket)) {
                        HostGameStatePacket* pkt = reinterpret_cast<HostGameStatePacket*>(event.packet->data);
                        if (pkt->type == PACKET_HOST_GAME_STATE) {
                            this->_latestState = *pkt;
                            this->_hasNewState = true;
                        }
                    } else if (event.packet->dataLength == sizeof(LobbyStatusPacket)) {
                        LobbyStatusPacket* pkt = reinterpret_cast<LobbyStatusPacket*>(event.packet->data);
                        if (pkt->type == PACKET_LOBBY_STATUS) {
                            this->_latestLobbyStatus = *pkt;
                        }
                    }
                    enet_packet_destroy(event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    std::cout << "[Client] Disconnected from host." << std::endl;
                    this->_status = ClientConnectionStatus::DISCONNECTED;
                    this->_peer = nullptr;
                    break;
                default:
                    break;
            }
        }
    }

    void sendClientInput(float posY) {
        if (!this->_peer || this->_status != ClientConnectionStatus::CONNECTED) return;

        ClientInputPacket pkt;
        pkt.type = PACKET_CLIENT_INPUT;
        pkt.clientPosY = posY;

        ENetPacket* packet = enet_packet_create(&pkt, sizeof(ClientInputPacket), 0); // Unreliable for high-freq position
        enet_peer_send(this->_peer, 0, packet);
    }

    void disconnect() {
        if (this->_peer && this->_status == ClientConnectionStatus::CONNECTED) {
            enet_peer_disconnect(this->_peer, 0);
            ENetEvent event;
            while (enet_host_service(this->_client, &event, 100) > 0) {
                if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
                    break;
                }
            }
            enet_peer_reset(this->_peer);
            this->_peer = nullptr;
        }
        this->_status = ClientConnectionStatus::DISCONNECTED;
    }

    inline ClientConnectionStatus getStatus() const { return this->_status; }
    inline bool isConnected() const { return this->_status == ClientConnectionStatus::CONNECTED; }
    inline bool hasNewState() const { return this->_hasNewState; }
    inline HostGameStatePacket getLatestState() {
        this->_hasNewState = false;
        return this->_latestState;
    }
    inline LobbyStatusPacket getLatestLobbyStatus() const { return this->_latestLobbyStatus; }

private:
    ENetHost* _client;
    ENetPeer* _peer;
    ClientConnectionStatus _status;
    HostGameStatePacket _latestState;
    LobbyStatusPacket _latestLobbyStatus;
    bool _hasNewState;
};

#endif /* CLIENT_H */
