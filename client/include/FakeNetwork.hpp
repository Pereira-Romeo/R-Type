/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** FakeNetwork
*/

#pragma once

#include <iostream>
#include <string>

namespace client
{
    inline void sendToServer(const std::string &message)
    {
        std::cout << "[CLIENT -> SERVER] " << message << std::endl;
    }

    inline void fakeServerResponse(const std::string &message)
    {
        std::cout << "[SERVER -> CLIENT] (fake) " << message << std::endl;
    }
}
