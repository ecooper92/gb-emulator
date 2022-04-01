// GBEmulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "CPU.h"
#include "Memory.h"
#include "Display.h"

#include <chrono>
#include <thread>
#include <vector>
#include <fstream>
#include <iostream>

int main()
{
    //std::ifstream input("drmario.gb", std::ios::binary);
    std::ifstream input("tetris.gb", std::ios::binary);
    //std::ifstream input("red.gb", std::ios::binary);
    Memory memory(input);
    Registers registers;
    CPU cpu(memory, registers);
    Display display(memory);
    std::cout << memory.Get_Title() << std::endl;

    registers.PC(0x100);
    display.Initialize();


    auto last_time = std::chrono::high_resolution_clock::now();
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto now_time = std::chrono::high_resolution_clock::now();
        double delta_ms = std::chrono::duration<double, std::milli>(now_time - last_time).count();
        last_time = last_time;

        cpu.Update(delta_ms);
        display.Update();
    }

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
