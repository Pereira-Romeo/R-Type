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

class FrameBufferHandler {
    private:

        typedef struct frameData_s {
            int frame = 0;
            std::vector<std::string> datas;
        } frameData_t;

        std::array<frameData_t, MAX_FRAMES> framesBuff = std::array<frameData_t, MAX_FRAMES>();
        int read = 0; // tete decriture
        int write = 0; // tete de lecture
        int history = 0; // balise de la plus vieille donner save (proba deja consommer)
        int currFrame = 0;

    public:
        FrameBufferHandler();
        ~FrameBufferHandler();
        void Delay(std::string data);
        bool Rollback(frameData_t data);
        void updateFrame() {currFrame++;};

        void Display();
};
