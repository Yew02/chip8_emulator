#pragma once

#include <cstdint>
#include <stack>
#include <array>
#include <cstdlib>
#include <fstream>
 


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

        bool draw_flag; // draw flag

    public:
        chip8(); // constructor

        void chip8_init(); // initialize the chip8

        bool load_rom(const std::string rom_path); // load the rom

        uint16_t fetch_instruction(); // fetch instruction

        void decode_excute(uint16_t instruction); // decode and excute instruction

        uint8_t get_screent_pixels(int x, int y); // get the screen pixels
        
        void set_keypad(uint8_t key, uint8_t value); // set the keypad

        bool get_draw_flag(); // get the draw flag

        void clear_draw_flag(); // clear the draw flag

        void chip8_cycle(); // chip8 cycle

        uint8_t helper_functions(uint16_t instruction); // get the first four bits of the instruction 

};