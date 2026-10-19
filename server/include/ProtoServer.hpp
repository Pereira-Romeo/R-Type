/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** protoServer
*/

#pragma once
#include <asio.hpp>
#include <array>
#include <cstdint>
#include <random>
#include <iostream>
#include "../../common/include/Protocol.hpp"
#include "./ClientTable.hpp"

class Server {
    public:
        Server(asio::io_context& io, unsigned short port);

    private:
        struct EnemyInfo {
            std::uint16_t id;
            std::uint16_t y;
            std::chrono::steady_clock::time_point born;   // sert à calculer son x actuel (v * t = d)
        };

        void startReceive();
        void handlePacket(std::size_t size);
        void handleConnect();
        void handleInput(const proto::Header& header);
        void sendWelcome(const Client& client);
        void send(const asio::ip::udp::endpoint& to,
            proto::Type type, const std::vector<std::uint8_t>& payload);
        void broadcast(proto::Type type,
            const std::vector<std::uint8_t>& payload, const Client* except);
        void scheduleSpawn();
        void spawnEnemy();
        void handleEnemyKilled(const proto::Header& header);
        void cleanEnemies();
        float enemyX(const EnemyInfo& enemy) const;

        std::vector<EnemyInfo> _enemies;
        std::uint16_t _nextEnemyId = 0;
            
        asio::steady_timer _spawnTimer;
        std::mt19937 _rng{std::random_device{}()};
        asio::ip::udp::socket _socket;
        asio::ip::udp::endpoint _sender;
        std::array<std::uint8_t, proto::MAX_PACKET_SIZE> _buffer;
        ClientTable _clients;
};
