#include "Network.hpp"
#include <chrono>
#include <iostream>
#include <thread>

Network::Network() : _socket(_io)
{
    _socket.open(asio::ip::udp::v4());
    _socket.non_blocking(true);
}

bool Network::waitPacket(std::size_t& size, int timeoutMs)
{
    auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs);

    while (std::chrono::steady_clock::now() < deadline) {
        asio::ip::udp::endpoint from;
        std::error_code ec;
        size = _socket.receive_from(asio::buffer(_buffer), from, 0, ec);

        // is a server paquet
        if (!ec && from == _server)
            return true;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    return false;
}

bool Network::connect(const std::string& ip, unsigned short port)
{
    std::error_code ec;
    auto address = asio::ip::make_address(ip, ec);
    if (ec) {
        std::cerr << "adresse invalide : " << ip << std::endl;
        return false;
    }

    // server adress
    _server = asio::ip::udp::endpoint(address, port);

    // 10 essai
    for (int attempt = 1; attempt <= 10; attempt++) {
        sendConnect();

        std::size_t size = 0;
        if (!waitPacket(size, 500))
            continue;

        proto::Header header;
        if (!proto::parseHeader(_buffer.data(), size, header))
            continue;

        if (header.type == proto::Type::ConnectRefused) {
            std::cerr << "serveur plein" << std::endl;
            return false;
        }
        if (header.type == proto::Type::Welcome && parseWelcome(header.payloadSize))
            return true;
    }
    std::cerr << "serveur injoignable" << std::endl;
    return false;
}

// Decod WELCOME and verifie the size 
bool Network::parseWelcome(std::size_t payloadSize)
{
    const std::uint8_t* p = _buffer.data() + proto::HEADER_SIZE;

    if (payloadSize < 9)
        return false;

    std::uint8_t count = p[8];
    if (count > proto::MAX_PLAYERS - 1 || payloadSize != 9u + count * 4u)
        return false;

    _welcome.player = {p[0], p[1], p[2], p[3]};
    _welcome.width = proto::read16(p + 4);
    _welcome.height = proto::read16(p + 6);

    _welcome.others.clear();
    for (int i = 0; i < count; i++) {
        const std::uint8_t* o = p + 9 + i * 4;
        _welcome.others.push_back({o[0], o[1], o[2], o[3]});
    }
    return true;
}

// get event each frame and transform in event
std::vector<NetEvent> Network::poll()
{
    std::vector<NetEvent> events;

    // garde-fou : max 64 paquets par frame
    for (int i = 0; i < 64; i++) {
        asio::ip::udp::endpoint from;
        std::error_code ec;
        std::size_t size = _socket.receive_from(asio::buffer(_buffer), from, 0, ec);

        if (ec)
            break;
        if (from != _server)
            continue;

        proto::Header header;
        if (!proto::parseHeader(_buffer.data(), size, header))
            continue;

        NetEvent event;
        if (decode(header, event))
            events.push_back(event);
    }
    return events;
}

// transform header to netevent
bool Network::decode(const proto::Header& header, NetEvent& out)
{
    const std::uint8_t* p = _buffer.data() + proto::HEADER_SIZE;

    switch (header.type) {
    case proto::Type::PlayerJoined:
        if (header.payloadSize != 4)
            return false;
        out.kind = NetEvent::Kind::PlayerJoined;
        out.id = p[0];
        out.r = p[1];
        out.g = p[2];
        out.b = p[3];
        return true;

    case proto::Type::PlayerInput:
        if (header.payloadSize != 2)
            return false;
        out.kind = NetEvent::Kind::PlayerInput;
        out.id = p[0];
        out.mask = p[1] & proto::INPUT_MASK;
        return true;

    case proto::Type::EnemySpawn:
        if (header.payloadSize != 6)
            return false;
        out.kind = NetEvent::Kind::EnemySpawn;
        out.enemyId = proto::read16(p);
        out.x = proto::read16(p + 2);
        out.y = proto::read16(p + 4);
        return true;

    case proto::Type::EnemyDead:
        if (header.payloadSize != 2)
            return false;
        out.kind = NetEvent::Kind::EnemyDead;
        out.enemyId = proto::read16(p);
        return true;

    default:
        return false;
    }
}

void Network::sendPacket(proto::Type type, const std::vector<std::uint8_t>& payload)
{
    std::vector<std::uint8_t> packet(proto::HEADER_SIZE + payload.size());
    proto::writeHeader(packet.data(), {type, static_cast<std::uint16_t>(payload.size())});
    std::copy(payload.begin(), payload.end(), packet.begin() + proto::HEADER_SIZE);

    std::error_code ec;
    _socket.send_to(asio::buffer(packet), _server, 0, ec);
}

void Network::sendConnect()
{
    sendPacket(proto::Type::Connect);
}

void Network::sendInput(std::uint8_t mask)
{
    sendPacket(proto::Type::Input, {static_cast<std::uint8_t>(mask & proto::INPUT_MASK)});
}

void Network::sendEnemyKilled(std::uint16_t enemyId)
{
    std::vector<std::uint8_t> payload;
    proto::append16(payload, enemyId);
    sendPacket(proto::Type::EnemyKilled, payload);
}