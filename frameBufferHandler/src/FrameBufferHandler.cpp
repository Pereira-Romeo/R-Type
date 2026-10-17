/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** FrameBufferHandler
*/

#include "../include/FrameBufferHandler.hpp"

FrameBufferHandler::FrameBufferHandler()
{}

FrameBufferHandler::~FrameBufferHandler()
{}

void FrameBufferHandler::Display()
{
    std::cout << "=== buffer (curr = " << _currFrame
              << ", write = " << _write
              << ", historique = " << _history << ") ===" << std::endl;

    for (int i = 0; i < MAX_FRAMES; i++) {
        std::cout << "[" << i << "]";

        if (i == _currFrame % MAX_FRAMES)
            std::cout << " <- curr";
        if (i == _history % MAX_FRAMES)
            std::cout << " <- historique";
        if (i == _read % MAX_FRAMES)
            std::cout << " <- read";
        if (i == _write % MAX_FRAMES) 
            std::cout << " <- write";

        std::cout << " : frame " << framesBuff[i].frame << " | ";

        if (framesBuff[i].datas.empty())
            std::cout << "(vide)";
        for (const auto& d : framesBuff[i].datas)
            std::cout << "\"" << d << "\" ";

        std::cout << std::endl;
    }
}

void FrameBufferHandler::updateFrame()
{
    _currFrame++;
    _write = _currFrame + DELAY;

    framesBuff[_write % MAX_FRAMES] = {_write, std::vector<std::string>()};

    int idhistory = _write - MAX_FRAMES + 1;
    _history = idhistory < 0 ? 0 : idhistory;
}

void FrameBufferHandler::Delay(std::string data)
{
    frameData_t& slot = framesBuff[_write % MAX_FRAMES];

    if (slot.frame != _write)
        slot = {_write, std::vector<std::string>()};
    slot.datas.emplace_back(data);
}

bool FrameBufferHandler::Rollback(frameData_t data)
{
    if (data.frame < _history || data.frame > _write) {
        for (const auto& d : data.datas)
            Write(data.frame, d);
        return false;
    }

    frameData_t& cur = framesBuff[data.frame % MAX_FRAMES];

    if (cur.frame == data.frame)
        cur.datas.insert(cur.datas.end(), data.datas.begin(), data.datas.end());
    else
        cur = data;

    return true;
}

std::vector<std::string> FrameBufferHandler::Read()
{
    if (_read < _history - 1)
        _read = _history - 1;

    int newRead = _read + 1;
    if (newRead > _currFrame)
        return std::vector<std::string>();
    _read = newRead;

    const frameData_t& slot = framesBuff[_read % MAX_FRAMES];
    if (slot.frame == _read && !slot.datas.empty())
        return slot.datas;
    return std::vector<std::string>();
}

void FrameBufferHandler::Write(int frame, std::string data)
{
    frameData_t& slot = framesBuff[frame % MAX_FRAMES];

    if (slot.frame != frame)
        slot = {frame, std::vector<std::string>()};
    slot.datas.emplace_back(std::move(data));
}
