#include "multiplayer.h"
#include <iostream>

Multiplayer::Multiplayer(Graphics& p_graphics, std::shared_ptr<Player> p_playerClient, std::shared_ptr<Player> p_playerHost, Hud& p_hud, const std::string& p_ipAddress) :
    _playerClient(p_playerClient),
    _playerHost(p_playerHost),
    _hud(p_hud),
    _collisionTimer(0.0f),
    _ball(p_graphics, Vector2f(globals::SCREEN_WIDTH / 2.0f, globals::SCREEN_HEIGHT / 2.0f)),
    _host(nullptr),
    _client(nullptr),
    _isHost(false)
{
    if (this->_playerHost != nullptr && this->_playerClient == nullptr) {
        // We are Host
        this->_isHost = true;
        this->_playerClient = std::make_shared<Player>(p_graphics, Vector2f(100, 100));
        this->_host = std::make_unique<Host>(7777);
    } else if (this->_playerClient != nullptr && this->_playerHost == nullptr) {
        // We are Client
        this->_isHost = false;
        this->_playerHost = std::make_shared<Player>(p_graphics, Vector2f(540, 100));
        this->_client = std::make_unique<Client>();
        this->_client->connectToHost(p_ipAddress.c_str(), 7777);
    }
}

bool Multiplayer::isConnected() const {
    if (this->_isHost && this->_host) {
        return this->_host->isClientConnected();
    } else if (!this->_isHost && this->_client) {
        return this->_client->isConnected();
    }
    return false;
}

void Multiplayer::draw(Graphics& p_graphics) {
    if (this->_playerClient) {
        this->_playerClient->draw(p_graphics);
    }
    if (this->_playerHost) {
        this->_playerHost->draw(p_graphics);
    }
    this->_ball.draw(p_graphics);
    this->_hud.renderPoints(this->_playerClient.get(), this->_playerHost.get());
}

void Multiplayer::update(float p_elapsedTime) {
    if (this->_collisionTimer > 0.0f) {
        this->_collisionTimer -= p_elapsedTime;
    }

    if (this->_isHost) {
        // Service host network events
        if (this->_host) {
            this->_host->update();
            if (this->_host->isClientConnected() && this->_host->hasClientInput()) {
                this->_playerClient->setY(this->_host->getClientPosY());
            }
        }

        // Host updates local paddle & ball physics
        if (this->_playerHost) {
            this->_playerHost->update(p_elapsedTime);
        }
        if (this->_playerClient) {
            this->_playerClient->update(p_elapsedTime);
        }

        // Ball movement and border bounces
        this->_ball.update(p_elapsedTime);

        // Paddle collisions
        this->handlePlayerBallCollision();

        // Check goals
        if (this->_ball.getX() <= 0) {
            if (this->_playerHost) this->_playerHost->gainPoint();
            this->_ball.reset();
        } else if (this->_ball.getX() + 10.0f >= globals::SCREEN_WIDTH) {
            if (this->_playerClient) this->_playerClient->gainPoint();
            this->_ball.reset();
        }

        // Broadcast state to client
        if (this->_host && this->_host->isClientConnected()) {
            HostGameStatePacket state;
            state.type = PACKET_HOST_GAME_STATE;
            state.hostPosY = this->_playerHost ? this->_playerHost->getY() : 100.0f;
            state.clientPosY = this->_playerClient ? this->_playerClient->getY() : 100.0f;
            state.ballX = this->_ball.getX();
            state.ballY = this->_ball.getY();
            state.hostPoints = this->_playerHost ? this->_playerHost->getPoints() : 0;
            state.clientPoints = this->_playerClient ? this->_playerClient->getPoints() : 0;
            state.gameStatus = (this->_playerClient && this->_playerClient->getLostStatus()) || 
                               (this->_playerHost && this->_playerHost->getLostStatus()) ? 1 : 0;

            this->_host->broadcastGameState(state);
        }
    } else {
        // We are Client
        if (this->_client) {
            this->_client->update();

            // Update local client paddle movement and send to host
            if (this->_playerClient) {
                this->_playerClient->update(p_elapsedTime);
                this->_client->sendClientInput(this->_playerClient->getY());
            }

            // Sync from host state
            if (this->_client->hasNewState()) {
                HostGameStatePacket state = this->_client->getLatestState();
                if (this->_playerHost) {
                    this->_playerHost->setY(state.hostPosY);
                    this->_playerHost->setPoints(state.hostPoints);
                }
                if (this->_playerClient) {
                    this->_playerClient->setPoints(state.clientPoints);
                }
                this->_ball.setPosition(state.ballX, state.ballY);
            }
        }
    }
}

void Multiplayer::handlePlayerBallCollision() {
    if (this->_collisionTimer > 0) return;

    const SpriteRectangle ballRect = this->_ball.getBoundingBox();

    if (this->_playerClient && ballRect.collidesWith(this->_playerClient->getBoundingBox())) {
        this->_ball.reverseDirectionX();
        this->_ball.setPosition(this->_playerClient->getBoundingBox().getRight() + 1.0f, this->_ball.getY());
        this->_collisionTimer = COLLISION_COOLDOWN_TIME;
    } else if (this->_playerHost && ballRect.collidesWith(this->_playerHost->getBoundingBox())) {
        this->_ball.reverseDirectionX();
        this->_ball.setPosition(this->_playerHost->getBoundingBox().getLeft() - ballRect.getWidth() - 1.0f, this->_ball.getY());
        this->_collisionTimer = COLLISION_COOLDOWN_TIME;
    }
}