#include <sdl2/sdl.h>
#include <iostream>
#include <array>
#include <cstdint>
#include <unordered_map>

#include "chip8.hpp"


const int SCREEN_WIDTH = 64;
const int SCREEN_HEIGHT = 32;
const int PIXEL_SIZE = 10; // size of each pixel

int main(int argc, char* argv[])
{
    //read the rom file
    if (argc <= 1)
    {
        std::cerr << "no specific rom file(use -h to get help)" << std::endl;
        exit(1);
    }

    //set up chip8 emulator
    chip8 chip8_emu;
    chip8_emu.chip8_init();

    //parse the arguments
    if(argc == 2)
    {
        if(std::string(argv[1]) == "-h")
        {
            std::cout << "Usage: ./chip8 <rom file>" << std::endl;
            exit(0);
        }else
        {
            if(!chip8_emu.load_rom(argv[1]))
            {
                std::cerr << "Failed to load the rom file(use -h to get help)" << std::endl;
                exit(1);
            }
        }
    }

    //initialize the screen
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    //create window
    SDL_Window* window = SDL_CreateWindow("Chip8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * PIXEL_SIZE, SCREEN_HEIGHT * PIXEL_SIZE, SDL_WINDOW_SHOWN);

    if (window == NULL)
    {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    //create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL)
    {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    //clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    //emulator loop
    bool quit = false;
    SDL_Event e;

    //link the keymap with the chip8 keypad
    const std::unordered_map<SDL_Scancode, uint8_t> keyMap = {
        { SDL_SCANCODE_1, 0x1 }, { SDL_SCANCODE_2, 0x2 }, { SDL_SCANCODE_3, 0x3 }, { SDL_SCANCODE_4, 0xC },
        { SDL_SCANCODE_Q, 0x4 }, { SDL_SCANCODE_W, 0x5 }, { SDL_SCANCODE_E, 0x6 }, { SDL_SCANCODE_R, 0xD },
        { SDL_SCANCODE_A, 0x7 }, { SDL_SCANCODE_S, 0x8 }, { SDL_SCANCODE_D, 0x9 }, { SDL_SCANCODE_F, 0xE },
        { SDL_SCANCODE_Z, 0xA }, { SDL_SCANCODE_X, 0x0 }, { SDL_SCANCODE_C, 0xB }, { SDL_SCANCODE_V, 0xF }
    };

    while(!quit)
    {
        //emulate one cycle
        chip8_emu.chip8_cycle();

        while(SDL_PollEvent(&e) != 0)
        {

            if(e.type == SDL_QUIT)
            {
                quit = true;
            }else if(e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
            {
                //std::cout << e.key.keysym.scancode << " key pressed" << std::endl;
                bool pressed = (e.type == SDL_KEYDOWN);
                auto key = keyMap.find(e.key.keysym.scancode);

                if(key != keyMap.end())
                {
                    //std::cout << key->first << " | " << key->second << std::endl;
                    chip8_emu.set_keypad(key->second, pressed);
                }
            }
        }
        if(chip8_emu.get_draw_flag())
        {
            chip8_emu.clear_draw_flag();
        }
        else
        {
            continue;
        }

        for (int y = 0; y < SCREEN_HEIGHT; ++y)
        {
            for (int x = 0; x < SCREEN_WIDTH; ++x)
            {
                if (chip8_emu.get_screent_pixels(x, y) == 1)
                {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                }

                SDL_Rect pixel = {x * PIXEL_SIZE, y * PIXEL_SIZE, PIXEL_SIZE, PIXEL_SIZE};
                SDL_RenderFillRect(renderer, &pixel);
            }
        }

        //update the screen
        SDL_RenderPresent(renderer);

        //sleep for 1/500 seconds
        SDL_Delay(1000 / 60);   
    }

    //clear the resources
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;

}


