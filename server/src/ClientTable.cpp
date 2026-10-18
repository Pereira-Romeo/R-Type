/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ClientTable
*/

#include "../include/ClientTable.hpp"

static const std::uint8_t PALETTE[proto::MAX_PLAYERS][3] = {
    {255, 0, 0}, // red
    {0, 255, 0}, // green
    {0, 100, 255}, // blue
    {255, 255, 0} // yellow
};

Client *ClientTable::find(const asio::ip::udp::endpoint &endpoint)
{
    for (auto &client : _clients) {
        if (client.used && client.endpoint == endpoint)
            return &client;
    }
    return nullptr;
}

Client *ClientTable::add(const asio::ip::udp::endpoint &endpoint)
{
    for (std::size_t i = 0; i < _clients.size(); i++) {
        if (_clients[i].used)
            continue;

        _clients[i].used = true;
        _clients[i].endpoint = endpoint;
        _clients[i].id = static_cast<std::uint8_t>(i);
        _clients[i].r = PALETTE[i][0];
        _clients[i].g = PALETTE[i][1];
        _clients[i].b = PALETTE[i][2];
        return &_clients[i];
    }
    return nullptr;
}

const std::array<Client, proto::MAX_PLAYERS>& ClientTable::all() const
{
    return _clients;
}

bool ClientTable::hasClients() const
{
    for (const auto &c : _clients)
        if (c.used)
            return true;
    return false;
}
