/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Network
*/

#pragma once
#include <asio.hpp>
#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include "../../common/include/Protocol.hpp"

struct PlayerInfo {
    std::uint8_t id;
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
};

struct WelcomeData {
    PlayerInfo player;
    std::uint16_t width = 0;
    std::uint16_t height = 0;
    std::vector<PlayerInfo> others;
};

struct NetEvent {
    enum class Kind { PlayerJoined, PlayerInput, EnemySpawn, EnemyDead };
    Kind kind;
    std::uint8_t id = 0;
    std::uint8_t r = 0;
    std::uint8_t g = 0;
    std::uint8_t b = 0;
    std::uint8_t mask = 0;
    std::uint16_t enemyId = 0;
    std::uint16_t x = 0;
    std::uint16_t y = 0;
};

class Network {
    public:
        Network();

        // Send CONNECT, wait WELCOME, false or echec if server refus
        bool connect(const std::string& ip, unsigned short port);
        const WelcomeData& getWelcome() const {return _welcome;};
        void sendInput(std::uint8_t mask);
        std::vector<NetEvent> poll();
        void sendEnemyKilled(std::uint16_t enemyId);

    private:
        void sendPacket(proto::Type type, const std::vector<std::uint8_t>& payload = {});
        void sendConnect();
        bool waitPacket(std::size_t& size, int timeoutMs);
        bool parseWelcome(std::size_t payloadSize);
        bool decode(const proto::Header& header, NetEvent& out);

        asio::io_context _io;
        asio::ip::udp::socket _socket;
        asio::ip::udp::endpoint _server;
        std::array<std::uint8_t, proto::MAX_PACKET_SIZE> _buffer;
        WelcomeData _welcome;
};
