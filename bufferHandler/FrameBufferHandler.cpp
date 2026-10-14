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
        if (i == read % MAX_FRAMES)
            std::cout << " <- read";

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
    int slot = data.frame - DELAY;

    if (slot < 0 || data.frame >= currFrame
        || data.frame < framesBuff[history % MAX_FRAMES].frame)
        return false;

    frameData_t& cur = framesBuff[slot % MAX_FRAMES];

    if (cur.frame == data.frame) {
        cur.datas.insert(cur.datas.end(),
                        data.datas.begin(),
                        data.datas.end());
    } else {
        cur = data;
    }

    return true;
}

std::string FrameBufferHandler::Read()
{
    int newRead = read + 1;          // numéro absolu, plus de modulo
    if (newRead > currFrame)         // on ne lit pas plus loin que ce qui est écrit
        return std::string("");
    read = newRead;
    return framesBuff[read % MAX_FRAMES].datas[0];
}
