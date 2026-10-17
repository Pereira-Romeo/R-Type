/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** FrameBufferHandler
*/

#pragma once
#include <string>
#include <array>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>

#define DELAY 2
#define MAX_FRAMES 10

typedef struct frameData_s {
    int frame = 0;
    std::vector<std::string> datas;
} frameData_t;

class FrameBufferHandler {
    private:
        std::array<frameData_t, MAX_FRAMES> framesBuff = std::array<frameData_t, MAX_FRAMES>();
        int _read = 0;
        int _write = DELAY;
        int _currFrame = 0;
        int _history = 0;

    public:
        FrameBufferHandler();
        ~FrameBufferHandler();
        void Delay(std::string data);
        bool Rollback(frameData_t data);
        void updateFrame();
        std::vector<std::string> Read();
        void Write(int frame, std::string data);
        void Display();
};
