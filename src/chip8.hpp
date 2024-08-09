#pragma once

#include <iostream>
#include <cstdint>
#include <stack>
#include <array>
#include <cstdlib>

 


class chip8 
{
    private:
        //cpu
        uint16_t pc_counter; // program counter
        uint16_t I; // index register
        std::stack<uint16_t> stack_memory; // stack
        uint8_t delay_timer, sound_timer; // delay timer ,sound timer
        std::array<uint8_t, 16> V; // registers

        //memory
        std::array<uint8_t, 4096> memory;

        //display
        std::array<uint8_t, 64 * 32> display;

        //keypad
        std::array<uint8_t, 16> keypad;
        
        //fontset
        std::array<uint8_t, 80> fontset;

    public:
       chip8(); // constructor

       uint16_t fetch_instruction(); // fetch instruction

       void decode_excute(uint16_t instruction); // decode and excute instruction

       uint8_t helper_functions(uint16_t instruction); // get the first four bits of the instruction 

};