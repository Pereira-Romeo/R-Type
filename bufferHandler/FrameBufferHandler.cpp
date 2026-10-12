/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** FrameBufferHandler
*/

#include "./FrameBufferHandler.hpp"

FrameBufferHandler::FrameBufferHandler()
{}

FrameBufferHandler::~FrameBufferHandler()
{}

void FrameBufferHandler::Display()
{
    std::cout << "=== buffer (curr = " << currFrame
              << ", historique = " << history << ") ===" << std::endl;

    for (int i = 0; i < MAX_FRAMES; i++) {
        std::cout << "[" << i << "]";

        if (i == currFrame % MAX_FRAMES)
            std::cout << " <- curr";
        if (i == history % MAX_FRAMES)
            std::cout << " <- historique";

        std::cout << " : frame " << framesBuff[i].frame << " | ";

        if (framesBuff[i].datas.empty())
            std::cout << "(vide)";
        for (const auto& d : framesBuff[i].datas)
            std::cout << "\"" << d << "\" ";

        std::cout << std::endl;
    }
}

void FrameBufferHandler::Delay(std::string data)
{
    if (currFrame >= history) {
        int idhistory = currFrame - MAX_FRAMES + 1;
        history =  idhistory < 0 ? 0 : idhistory;
    }

    frameData_t newData = {currFrame + DELAY, std::vector<std::string>()};
    newData.datas.emplace_back(data);
    framesBuff[currFrame % MAX_FRAMES] = newData;
}

bool FrameBufferHandler::Rollback(frameData_t data)
{
    // if (data.frame <= currFrame)
    //     return false;
    
    // framesBuff[(data.frame - DELAY) % MAX_FRAMES].datas.emplace_back(data);
    
    // return true;
}
