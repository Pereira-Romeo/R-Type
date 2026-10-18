/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ProtoServer
*/

#include "../include/ProtoServer.hpp"

Server::Server(asio::io_context& io, unsigned short port)
    : _spawnTimer(io), _socket(io, asio::ip::udp::endpoint(asio::ip::udp::v4(), port))
{
    startReceive();
    scheduleSpawn();
}

// Wait for the first message 
void Server::startReceive()
{
    _socket.async_receive_from(
        asio::buffer(_buffer), _sender,
        [this](std::error_code ec, std::size_t size) {
            if (!ec)
                handlePacket(size);
            startReceive();
        });
}

// Check the type of packet
void Server::handlePacket(std::size_t size)
{
    proto::Header header;
    if (!proto::parseHeader(_buffer.data(), size, header)) {
        std::cout << "paquet invalide jeté (" << size << " octets)" << std::endl;
        return;
    }

    switch (header.type) {
        case proto::Type::Connect:
            handleConnect();
            break;
        case proto::Type::Input:
            handleInput(header);
            break;
        case proto::Type::EnemyKilled:
            handleEnemyKilled(header);
            break;
        default:
            break;
    }
}

// Connect the client 
void Server::handleConnect()
{
    Client* client = _clients.find(_sender);
    bool isNew = false;

    if (!client) {
        client = _clients.add(_sender);
        isNew = true;
    }

    if (!client) {
        send(_sender, proto::Type::ConnectRefused, {});
        std::cout << "serveur plein, refusé : " << _sender << std::endl;
        return;
    }

    // Confirm the connection
    sendWelcome(*client);

    if (isNew) {
        std::vector<std::uint8_t> payload;
        payload.push_back(client->id);
        payload.push_back(client->r);
        payload.push_back(client->g);
        payload.push_back(client->b);
        broadcast(proto::Type::PlayerJoined, payload, client);

        for (const auto &e : _enemies) {
            float x = std::max(0.0f, enemyX(e));    // jamais négatif avant la conversion
            std::vector<std::uint8_t> p;
            proto::append16(p, e.id);
            proto::append16(p, static_cast<std::uint16_t>(x));
            proto::append16(p, e.y);
            send(client->endpoint, proto::Type::EnemySpawn, p);
        }
    }
}

// Send data to client
void Server::send(const asio::ip::udp::endpoint& to,
    proto::Type type, const std::vector<std::uint8_t>& payload)
{
    auto packet = std::make_shared<std::vector<std::uint8_t>>(proto::HEADER_SIZE + payload.size());

    proto::Header header{type, static_cast<std::uint16_t>(payload.size())};
    proto::writeHeader(packet->data(), header);
    std::copy(payload.begin(), payload.end(), packet->begin() + proto::HEADER_SIZE);

    _socket.async_send_to(asio::buffer(*packet), to, [packet](std::error_code, std::size_t) {});
}

// Send the confirm to the client
void Server::sendWelcome(const Client& client)
{
    std::vector<std::uint8_t> payload;

    // id + color
    payload.push_back(client.id);
    payload.push_back(client.r);
    payload.push_back(client.g);
    payload.push_back(client.b);

    // ecrant
    proto::append16(payload, proto::SCREEN_WIDTH);
    proto::append16(payload, proto::SCREEN_HEIGHT);

    // other player
    payload.push_back(0);
    std::size_t countPos = payload.size() - 1;
    for (const auto& other : _clients.all()) {
        if (!other.used || &other == &client)
            continue;
        payload.push_back(other.id);
        payload.push_back(other.r);
        payload.push_back(other.g);
        payload.push_back(other.b);
        payload[countPos]++;
    }

    send(client.endpoint, proto::Type::Welcome, payload);
    std::cout << "WELCOME envoyé au client " << int(client.id) << std::endl;
}

void Server::handleInput(const proto::Header& header)
{
    if (header.payloadSize != 1)
        return;

    Client* client = _clients.find(_sender);
    if (!client)
        return;

    std::uint8_t mask = _buffer[proto::HEADER_SIZE] & proto::INPUT_MASK;
 
    std::vector<std::uint8_t> payload;
    payload.push_back(client->id);
    payload.push_back(mask);
    broadcast(proto::Type::PlayerInput, payload, client);
}

// send mess to all client except the sender
void Server::broadcast(proto::Type type, const std::vector<std::uint8_t>& payload, const Client* except)
{
    for (const auto& client : _clients.all()) {
        if (!client.used || &client == except)
            continue;
        send(client.endpoint, type, payload);
    }
}


void Server::scheduleSpawn()
{
    _spawnTimer.expires_after(std::chrono::seconds(2));
    _spawnTimer.async_wait([this](std::error_code ec) {
        if (!ec)
            spawnEnemy();
        scheduleSpawn();
    });
}

void Server::spawnEnemy()
{
    if (!_clients.hasClients())
        return;

    cleanEnemies();
    if (_enemies.size() >= proto::MAX_ENEMIES)    // garde-fou mémoire
        return;

    std::uniform_int_distribution<int> distY(0, proto::SCREEN_HEIGHT - 50);

    EnemyInfo enemy{_nextEnemyId++, static_cast<std::uint16_t>(distY(_rng)),
                    std::chrono::steady_clock::now()};
    _enemies.push_back(enemy);

    std::vector<std::uint8_t> payload;
    proto::append16(payload, enemy.id);
    proto::append16(payload, proto::SCREEN_WIDTH);
    proto::append16(payload, enemy.y);
    broadcast(proto::Type::EnemySpawn, payload, nullptr);
}

float Server::enemyX(const EnemyInfo& enemy) const
{
    float seconds = std::chrono::duration<float>(
        std::chrono::steady_clock::now() - enemy.born).count();
    return proto::SCREEN_WIDTH - proto::ENEMY_SPEED * seconds;
}

// Retire les ennemis sortis de l'écran
void Server::cleanEnemies()
{
    std::erase_if(_enemies, [this](const EnemyInfo& e) {
        return enemyX(e) <= 0.0f;
    });
}

void Server::handleEnemyKilled(const proto::Header& header)
{
    if (header.payloadSize != 2)
        return;

    Client* client = _clients.find(_sender);
    if (!client)
        return;

    std::uint16_t id = proto::read16(_buffer.data() + proto::HEADER_SIZE);

    auto it = std::ranges::find(_enemies, id, &EnemyInfo::id);
    if (it == _enemies.end())
        return;

    _enemies.erase(it);

    std::vector<std::uint8_t> payload;
    proto::append16(payload, id);
    broadcast(proto::Type::EnemyDead, payload, nullptr);
}
