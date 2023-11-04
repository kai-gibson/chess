// seperate exe for generating all path moves (precomputing 
// to save cycles)

#include <array>
#include <iostream>
#include <string>
#include <iomanip>
#include "../print_board.h" 

using i8 = char;
using u8 = unsigned char;
using u64 = unsigned long;

enum { TOP, TOP_RIGHT, RIGHT, BOTTOM_RIGHT, 
       BOTTOM, BOTTOM_LEFT, LEFT, TOP_LEFT };

std::array<std::array<u64, 8>, 64> moves_list;

u64 get_bin_num(i8 arr_indx) { 
    return u64(1) << arr_indx;
}

void gen_moves_list() {
    for (i8 i=0; i<u8(moves_list.size()); i++) {
        u64 piece_pos = get_bin_num(i);

        // Get top moves
        u8 move_loc = i; 
        u8 counter = 0;
        while (move_loc/8 < 7) {
            move_loc+=8; 
            counter+=8;
            moves_list[i][TOP] += piece_pos << counter;
        }
        
        // Get top right moves
        move_loc = i; 
        counter = 0;
        while ((move_loc%8 > 0) && (move_loc/8)<7) {
            move_loc+=7; 
            counter+=7;
            moves_list[i][TOP_RIGHT] += piece_pos << counter;
        }

        // Get right moves
        move_loc = i;
        counter = 0;
        while (move_loc % 8 > 0) {
            move_loc-=1;
            counter+=1;
            moves_list[i][RIGHT] += piece_pos >> counter;
        }

        // Get bottom right moves
        move_loc = i; 
        counter = 0;
        while ((move_loc%8 > 0) && (move_loc/8)>0) {
            move_loc-=9; 
            counter+=9;
            moves_list[i][BOTTOM_RIGHT] += piece_pos >> counter;
        }

        // Get bottom moves
        move_loc = i;
        counter = 0;
        while (move_loc/8 > 0) {
            move_loc-=8;
            counter+=8;
            moves_list[i][BOTTOM] += piece_pos >> counter;
        }

        // Get bottom left moves
        move_loc = i; 
        counter = 0;
        while ((move_loc%8 < 7) && (move_loc/8>0)) {
            move_loc -= 7; 
            counter += 7;
            moves_list[i][BOTTOM_LEFT] += piece_pos >> counter;
        }

        // Get left moves
        move_loc = i;
        counter = 0;
        while (move_loc % 8 < 7) {
            move_loc+=1;
            counter+=1;
            moves_list[i][LEFT] += piece_pos << counter;
        }

        // Get top left moves
        move_loc = i; // index from top
        counter = 0;
        while ((move_loc%8 < 7) && (move_loc/8)<7) {
            move_loc += 9; 
            counter += 9;
            moves_list[i][TOP_LEFT] += piece_pos << counter;
        }
    }
}

int main(int argc, char** argv) {
    size_t pos = 0;
    bool gen_all = false; 

    if (argc == 3) {
        if (std::string(argv[1]) == "for") {
            if (std::string(argv[2]) == "all") {
                gen_all = true;
            } else {
                gen_all = false;
                try {
                    pos = std::stoi(argv[2]); 
                } catch (std::invalid_argument) {
                    std::cerr << "ERROR: non-numeric argument supplied\n";
                    return -1;
                }
            }
        }
    } else {
        std::cerr << "Incorrect amount of arguments\n" 
                  << "Usage: ./gen_moves_list for 0\n"
                  << "    Or: ./gen_moves_list for all\n";
    }

    gen_moves_list();
    
    std::array<std::string, 8> directions = { 
       "TOP", "TOP_RIGHT", "RIGHT", "BOTTOM_RIGHT", 
       "BOTTOM", "BOTTOM_LEFT", "LEFT", "TOP_LEFT"
    };

    if (!gen_all) {
        for (size_t i=0; i < moves_list[pos].size(); i++) {
            std::cout << directions[i] << "\n";
            print_piece_map(moves_list[pos][i]);
        }
    } else {
        std::cout << "std::array<std::array<u64, 8>, 64> moves_list = {\n";
        for (int i =0; i<moves_list.size(); i++) {
            std::cout << "    {\n";
            for (int j=0; j<moves_list[i].size(); j++) {
                std::cout << "        0x" << std::hex << std::uppercase 
                          << std::setfill('0') << std::setw(16)
                          << moves_list[i][j];

                if (j != moves_list[i].size()-1) std::cout << ",";
                std::cout << "\n";
            }
            std::cout << "    }";
            if (i != moves_list.size()-1) std::cout << ",";
            std::cout << "\n";
        }
        std::cout << "};\n";
    }
}
