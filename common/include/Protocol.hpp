/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Protocol
*/

#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace proto {

constexpr std::size_t HEADER_SIZE = 5;
constexpr std::size_t MAX_PACKET_SIZE = 1024;
constexpr std::size_t MAX_PLAYERS = 4;
constexpr std::uint16_t SCREEN_WIDTH = 1280;
constexpr std::uint16_t SCREEN_HEIGHT = 720;
constexpr std::size_t MAX_ENEMIES = 32;
constexpr float ENEMY_SPEED = 100.f;

constexpr std::uint8_t INPUT_UP = 1;
constexpr std::uint8_t INPUT_DOWN = 2;
constexpr std::uint8_t INPUT_LEFT = 4;
constexpr std::uint8_t INPUT_RIGHT = 8;
constexpr std::uint8_t INPUT_SHOOT = 16;
constexpr std::uint8_t INPUT_MASK = 31;


enum class Type : std::uint8_t {
    Connect = 1,
    Welcome = 2,
    ConnectRefused = 3,
    Input = 4,
    PlayerInput = 5,
    PlayerJoined = 6,
    EnemySpawn = 7,
    EnemyDead = 8,
    EnemyKilled = 9,
    EndEnum
};

struct Header {
    Type type;
    std::uint16_t payloadSize;
    std::uint16_t sequence;
};

// Reads the header. Returns false if the packet is invalid.
bool parseHeader(const std::uint8_t* data, std::size_t size, Header& out);

// Writes the header into out (at least HEADER_SIZE bytes).
void writeHeader(std::uint8_t* out, const Header& header);

// Appends a 16-bit integer (network byte order) to the end of a payload.
void append16(std::vector<std::uint8_t>& payload, std::uint16_t value);

// Reads a 16-bit integer (network byte order) at the given address.
std::uint16_t read16(const std::uint8_t* data);

inline float startX() { return 100.f; }
inline float startY(std::uint8_t id) { return 150.f + id * 120.f; }

}
