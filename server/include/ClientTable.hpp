/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ClientTable
*/

#pragma once
#include <asio.hpp>
#include <array>
#include <chrono>
#include <cstdint>
#include "../../common/include/Protocol.hpp"

struct Client {
    bool used = false;
    asio::ip::udp::endpoint endpoint; // IP + port of the player
    std::uint8_t id = 0;
    std::uint8_t r = 0;
    std::uint8_t g = 0;
    std::uint8_t b = 0;
};

class ClientTable {
    public:
        // Search a client in the table. send nullptr if not found
        Client *find(const asio::ip::udp::endpoint& endpoint);

        // Take a free place. nullptr if is full
        Client *add(const asio::ip::udp::endpoint& endpoint);

        // Return the array of player
        const std::array<Client, proto::MAX_PLAYERS>& all() const;

        bool hasClients() const;

    private:
        std::array<Client, proto::MAX_PLAYERS> _clients;
};
