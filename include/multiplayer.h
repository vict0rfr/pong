#ifndef MULTIPLAYER_H
#define MULTIPLAYER_H

#include "graphics.h"
#include "player.h"
#include "ball.h"
#include "hud.h"
#include "client.h"
#include "host.h"

#include <memory>

class Multiplayer {
public:
    Multiplayer(Graphics& p_graphics, std::shared_ptr<Player> p_client, std::shared_ptr<Player> p_host, Hud& p_hud, const std::string& p_ipAddress = "127.0.0.1");
    ~Multiplayer() = default;

    void draw(Graphics& p_graphics);
    void update(float p_elapsedTime);

    inline bool isHost() const { return this->_isHost; }
    bool isConnected() const;

private:
    std::shared_ptr<Player> _playerClient;
    std::shared_ptr<Player> _playerHost;
    Hud& _hud;
    Ball _ball;

    std::unique_ptr<Host> _host;
    std::unique_ptr<Client> _client;
    bool _isHost;

    const float COLLISION_COOLDOWN_TIME = 200.0f;
    float _collisionTimer;

    void handlePlayerBallCollision();
};

#endif // MULTIPLAYER_H
