#include "Protocol.hpp"

#include <cstring>

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <arpa/inet.h>
#endif


static bool isKnownType(std::uint8_t value)
{
    return value >= static_cast<std::uint8_t>(proto::Type::Connect)
        && value < static_cast<std::uint8_t>(proto::Type::EndEnum);
}

bool proto::parseHeader(const std::uint8_t* data, std::size_t size, Header& out)
{
    // Enough bytes, and not too big?
    if (size < HEADER_SIZE || size > MAX_PACKET_SIZE)
        return false;

    // Known type?
    if (!isKnownType(data[0]))
        return false;

    // Copy the raw bytes (avoids alignment/aliasing issues)
    std::uint16_t rawSize, rawSeq;
    std::memcpy(&rawSize, data + 1, 2);
    std::memcpy(&rawSeq, data + 3, 2);

    std::uint16_t payloadSize = ntohs(rawSize);
    if (payloadSize != size - HEADER_SIZE)
        return false;

    // Fill the output only once everything is validated
    out.type = static_cast<Type>(data[0]);
    out.payloadSize = payloadSize;
    out.sequence = ntohs(rawSeq);
    return true;
}

void proto::writeHeader(std::uint8_t* out, const Header& header)
{
    std::uint16_t size = htons(header.payloadSize);
    std::uint16_t seq = htons(header.sequence);

    out[0] = static_cast<std::uint8_t>(header.type);
    std::memcpy(out + 1, &size, 2);
    std::memcpy(out + 3, &seq, 2);
}

void append16(std::vector<std::uint8_t>& payload, std::uint16_t value)
{
    std::uint16_t net = htons(value);
    std::uint8_t bytes[2];
    std::memcpy(bytes, &net, 2);
    payload.push_back(bytes[0]);
    payload.push_back(bytes[1]);
}

std::uint16_t proto::read16(const std::uint8_t* data)
{
    std::uint16_t raw;
    std::memcpy(&raw, data, 2);
    return ntohs(raw);
}
