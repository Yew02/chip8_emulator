#include "chip8.hpp"

chip8::chip8()
{

    //initialize the cpu registers
    pc_counter = 0x200;
    I = 0;
    delay_timer = 0;
    sound_timer = 0;
    V.fill(0);

    //initialize the memory
    memory.fill(0);

    //initialize the display
    // 0 represents black , 255 represents white
    display.fill(0);


    //initialize the keypad
    keypad.fill(0);

    //initialize the fontset
    fontset = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
}

//fetch instruction
uint16_t chip8::fetch_instruction()
{
    uint16_t current_instruction = (memory[pc_counter] << 8) | memory[pc_counter + 1];
    pc_counter += 2;
    return current_instruction;
}


//decode and excute instruction
void chip8::decode_excute(uint16_t instruction)
{
    //get the first four bits of the instruction
    uint8_t first_nibble = helper_functions(instruction);

    switch(first_nibble)
    {
        //default = 0x0NNN 
        //fix command = 0x00E0, 0x00EE
        case 0x0:
        {
            switch(instruction)
            {
                //clear the display
                case 0x00E0:
                    display.fill(0);
                    break;

                //return from a subroutine
                case 0x00EE:
                    pc_counter = stack_memory.top();
                    stack_memory.pop();
                    break;

                //implement the 0NNN command(not implemented)
                default:
                    std::cerr << "0NNN command not implemented" << std::endl;
                    break;
            }
        }

        //jump to address NNN
        case 0x1:
        {
            pc_counter = instruction & 0x0FFF;
            break;
        }

        //call subroutine at NNN
        case 0x2:
        {
            stack_memory.push(pc_counter);
            pc_counter = instruction & 0x0FFF;
            break;
        }

        //3XNN
        //skip next instruction if VX == NN
        case 0x3:
        {
            if(V.at((instruction & 0x0F00) >> 8) == (instruction & 0x00FF))
            {
                pc_counter += 2;
            }
            break;
        }

        //4XNN
        //skip next instruction if VX != NN
        case 0x4:
        {
            if(V.at((instruction & 0x0F00) >> 8) != (instruction & 0x00FF))
            {
                pc_counter += 2;
            }
            break;
        }

        //5XY0
        //skip next instruction if VX == VY
        case 0x5:
        {
            if(V.at((instruction & 0x0F00) >> 8) == V.at((instruction & 0x00F0) >> 4))
            {
                pc_counter += 2;
            }
        }

        //6XNN
        //set VX = NN
        case 0x6:
        {
            V.at((instruction & 0x0F00) >> 8) = instruction & 0x00FF;
            break;
        }

        //7XNN
        //add VX = VX + NN, vf carry flag not changed
        case 0x7:
        {
            V.at((instruction & 0x0F00) >> 8) += instruction & 0x00FF;
            break;
        }

        //8XY0 , 8XY1, 8XY2, 8XY3, 8XY4, 8XY5, 8XY6, 8XY7, 8XYE
        //operations on VX and VY
        case 0x8:
        {
            //check the last four bits of the instruction
            switch(instruction & 0x000F)
            {
                //VX = VY
                case(0x0):
                {
                    V.at((instruction & 0x0F00) >> 8) = V.at((instruction & 0x00F0) >> 4);
                    break;
                }

                //VX = VX | VY
                case(0x1):
                {
                    V.at((instruction & 0x0F00) >> 8) |= V.at((instruction & 0x00F0) >> 4);
                    break;
                }

                //VX = VX & VY
                case(0x2):
                {
                    V.at((instruction & 0x0F00) >> 8) &= V.at((instruction & 0x00F0) >> 4);
                    break;
                }

                //VX = VX ^ VY
                case(0x3):
                {
                    V.at((instruction & 0x0F00) >> 8) ^= V.at((instruction & 0x00F0) >> 4);
                    break;
                }

                //VX = VX + VY, vf = carry flag
                case(0x4):
                {
                    uint16_t sum = V.at((instruction & 0x0F00) >> 8) + V.at((instruction & 0x00F0) >> 4);
                    V.at(0xF) = sum > 0xFF ? 1 : 0;
                    V.at((instruction & 0x0F00) >> 8) = sum & 0xFF;
                    break;
                }

                //VX = VX - VY, vf = not borrow(1 if VX > VY else 0)
                case(0x5):
                {
                    V.at(0xF) = V.at((instruction & 0x0F00) >> 8) > V.at((instruction & 0x00F0) >> 4) ? 1 : 0;
                    V.at((instruction & 0x0F00) >> 8) -= V.at((instruction & 0x00F0) >> 4);
                    break;
                }
                
                //VX = VX >> 1, VY(just ignore),VF = least significant bit of VX
                case(0x6):
                {
                    V.at(0xF) = V.at((instruction & 0x0F00) >> 8) & 0x1;
                    V.at((instruction & 0x0F00) >> 8) >>= 1;
                    break;
                }
                
                //VX = VY - VX, vf = not borrow(1 if VY > VX else 0)
                case(0x7):
                {
                    V.at(0xF) = V.at((instruction & 0x00F0) >> 4) > V.at((instruction & 0x0F00) >> 8) ? 1 : 0;
                    V.at((instruction & 0x0F00) >> 8) = V.at((instruction & 0x00F0) >> 4) - V.at((instruction & 0x0F00) >> 8);
                    break;
                }

                //VX = VX << 1, VY(just ignore),VF = least significant bit of VX
                case(0xE):
                {
                    V.at(0xF) = V.at((instruction & 0x0F00) >> 8) >> 7;
                    V.at((instruction & 0x0F00) >> 8) <<= 1;
                    break;
                }
            }
        }

        //9XY0
        //skip next instruction if VX != VY
        case(0x9):
        {
            if(V.at((instruction & 0x0F00) >> 8) != V.at((instruction & 0x00F0) >> 4))
            {
                pc_counter += 2;
            }
            break;
        }

        //AXNN
        //set I = NNN
        case(0xA):
        {
            I = instruction & 0x0FFF;
            break;
        }

        //BNNN
        //jump to address NNN + V0
        case(0xB):
        {
            pc_counter = (instruction & 0x0FFF) + V.at(0);
            break;
        }

        //CXNN
        //set VX = random byte AND NN
        case(0xC):
        {
            V.at((instruction & 0x0F00) >> 8) = (rand() % 0x100) & (instruction & 0x00FF);
            break;
        }

        //DXYN
        //display n-byte sprite starting at memory location I at (VX, VY), vf = collision
        case(0xD):
        {
            uint8_t x = V.at((instruction & 0x0F00) >> 8);
            uint8_t y = V.at((instruction & 0x00F0) >> 4);
            uint8_t height = instruction & 0x000F;
            for(int yline = 0; yline < height; yline++)
            {
                uint8_t pixel = memory.at(I + yline);
                for(int xline = 0; xline < 8; xline++)
                {
                    //check if the current pixel is set to 1
                    //if yes, then check if the display pixel is set to 1
                    if((pixel & (0x80 >> xline)) != 0)
                    {
                        if(display.at((x + xline + ((y + yline) * 64))) == 1)
                        {
                            V.at(0xF) = 1;
                        }
                        // xor the display pixel with 1(flip the bit)
                        display.at(x + xline + ((y + yline) * 64)) ^= 1;
                    }
                }
            }
            break;  
        }

        //EX9E, EXA1









    }
    
}


//helper function to get the first four bits of the instruction
uint8_t chip8::helper_functions(uint16_t instruction)
{
    return (instruction & 0xF000) >> 12;
}