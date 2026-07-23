#ifndef NET_PACKETS_H
#define NET_PACKETS_H

#include <cstdint>

enum PacketType : uint8_t {
    PACKET_CLIENT_INPUT = 1,
    PACKET_HOST_GAME_STATE = 2,
    PACKET_LOBBY_STATUS = 3
};

#pragma pack(push, 1)
struct ClientInputPacket {
    uint8_t type = PACKET_CLIENT_INPUT;
    float clientPosY = 0.0f;
};

struct HostGameStatePacket {
    uint8_t type = PACKET_HOST_GAME_STATE;
    float hostPosY = 0.0f;
    float clientPosY = 0.0f;
    float ballX = 0.0f;
    float ballY = 0.0f;
    int32_t hostPoints = 0;
    int32_t clientPoints = 0;
    uint8_t gameStatus = 0; // 0 = playing, 1 = host won, 2 = client won
};

struct LobbyStatusPacket {
    uint8_t type = PACKET_LOBBY_STATUS;
    uint8_t clientConnected = 0;
    uint8_t gameStarted = 0;
};
#pragma pack(pop)

#endif // NET_PACKETS_H
