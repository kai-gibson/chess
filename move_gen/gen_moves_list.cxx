// seperate exe for generating all path moves (precomputing 
// to save cycles)

#include <array>
#include <iostream>
#include <string>
#include <iomanip>
#include <algorithm>
#include "../print_board.h" 

using i8 = char;
using u8 = unsigned char;
using u64 = unsigned long;

enum { TOP, TOP_RIGHT, RIGHT, BOTTOM_RIGHT, 
       BOTTOM, BOTTOM_LEFT, LEFT, TOP_LEFT };

enum { WHITE_MOVE, WHITE_ATTACK, BLACK_MOVE, BLACK_ATTACK }; // pawn_moves

std::array<std::array<u64, 8>, 64> path_moves;
std::array<std::array<u64,4>, 64> pawn_moves;
std::array<u64, 64> king_moves;
std::array<u64, 64> knight_moves;

u64 get_bin_num(i8 arr_indx) { 
    return u64(1) << arr_indx;
}

void gen_moves_pawn() {
    for (i8 i=0; i < pawn_moves.size(); i++) {
        u64 piece_pos = get_bin_num(i);
        // Get white moves
        if (i/8 == 1) { // if first move, move once or twice
            pawn_moves[i][WHITE_MOVE] |= ((piece_pos << 8) 
                    | (piece_pos << 16));
        } 
        else if (i/8 < 7) {
            pawn_moves[i][WHITE_MOVE] |= (piece_pos << 8);
        }
        // Get white attack moves
        if (i/8 < 7 ) {
            // left attack
            if (i%8 < 7) {
                pawn_moves[i][WHITE_ATTACK] |= piece_pos << 9;
            }
            // right attack
            if (i%8 > 0) {
                pawn_moves[i][WHITE_ATTACK] |= piece_pos << 7;
            }
        }

        // Get black moves
        if (i/8 == 6) { // if first move, move once or twice
            pawn_moves[i][BLACK_MOVE] |= ((piece_pos >> 8) 
                    | (piece_pos >> 16));
        }
        else if (i/8 > 0) {
            pawn_moves[i][BLACK_MOVE] |= (piece_pos >> 8);
        }
        // Get black attack moves
        if (i/8 > 0 ) {
            // left attack
            if (i%8 < 7) {
                pawn_moves[i][BLACK_ATTACK] |= piece_pos >> 7;
            }
            // right attack
            if (i%8 > 0) {
                pawn_moves[i][BLACK_ATTACK] |= piece_pos >> 9;
            }
        }
    }
}

void gen_moves_king() {
    for (i8 i=0; i < king_moves.size(); i++) {
        u64 piece_pos = get_bin_num(i);

        // top moves
        if (i / 8 < 7) {
            king_moves[i] |= piece_pos << 8;

            // top right 
            if (i % 8 > 0) { 
                king_moves[i] |= piece_pos << 7;
            }

            // top left
            if (i % 8 < 7) {
                king_moves[i] |= piece_pos << 9;
            }
        }
        // bottom moves
        if (i / 8 > 0) {
            king_moves[i] |= piece_pos >> 8;

            // bottom right 
            if (i % 8 > 0) { 
                king_moves[i] |= piece_pos >> 9;
            }

            // bottom left
            if (i % 8 < 7) {
                king_moves[i] |= piece_pos >> 7;
            }
        }

        // right move
        if (i % 8 > 0) {
            king_moves[i] |= piece_pos >> 1;
        }

        // left move
        if (i % 8 < 7) {
            king_moves[i] |= piece_pos << 1;
        }
    }
}
void gen_moves_knight() {
    for (i8 i=0; i < knight_moves.size(); i++) {
        u64 piece_pos = get_bin_num(i);

        // top moves
        if (i/8 < 6) {
            // top left
            if (i%8 < 7) {
                knight_moves[i] |= piece_pos << 17;
            }
            // top right
            if (i%8 > 0) {
                knight_moves[i] |= piece_pos << 15;
            }
        }

        // left moves
        if (i%8 < 6) {
            // left top
            if (i/8 < 7) {
                knight_moves[i] |= piece_pos << 10;
            }
            // left bottom
            if (i/8 > 0) {
                knight_moves[i] |= piece_pos >> 6;
            }
        }
        // right moves
        if (i%8 > 1) {
            // right top
            if (i/8 < 7) {
                knight_moves[i] |= piece_pos << 6;
            }
            // right bottom
            if (i/8 > 0) {
                knight_moves[i] |= piece_pos >> 10;
            }
        }

        // bottom moves
        if (i/8 > 1) {
            // left
            if (i%8 < 7) {
                knight_moves[i] |= piece_pos >> 15;
            }
            // right
            if (i%8 > 0) {
                knight_moves[i] |= piece_pos >> 17;
            }
        }
    }
}

void gen_moves_path() {
    for (i8 i=0; i<u8(path_moves.size()); i++) {
        u64 piece_pos = get_bin_num(i);

        // Get top moves
        u8 move_loc = i; 
        u8 counter = 0;
        while (move_loc/8 < 7) {
            move_loc+=8; 
            counter+=8;
            path_moves[i][TOP] += piece_pos << counter;
        }
        
        // Get top right moves
        move_loc = i; 
        counter = 0;
        while ((move_loc%8 > 0) && (move_loc/8)<7) {
            move_loc+=7; 
            counter+=7;
            path_moves[i][TOP_RIGHT] += piece_pos << counter;
        }

        // Get right moves
        move_loc = i;
        counter = 0;
        while (move_loc % 8 > 0) {
            move_loc-=1;
            counter+=1;
            path_moves[i][RIGHT] += piece_pos >> counter;
        }

        // Get bottom right moves
        move_loc = i; 
        counter = 0;
        while ((move_loc%8 > 0) && (move_loc/8)>0) {
            move_loc-=9; 
            counter+=9;
            path_moves[i][BOTTOM_RIGHT] += piece_pos >> counter;
        }

        // Get bottom moves
        move_loc = i;
        counter = 0;
        while (move_loc/8 > 0) {
            move_loc-=8;
            counter+=8;
            path_moves[i][BOTTOM] += piece_pos >> counter;
        }

        // Get bottom left moves
        move_loc = i; 
        counter = 0;
        while ((move_loc%8 < 7) && (move_loc/8>0)) {
            move_loc -= 7; 
            counter += 7;
            path_moves[i][BOTTOM_LEFT] += piece_pos >> counter;
        }

        // Get left moves
        move_loc = i;
        counter = 0;
        while (move_loc % 8 < 7) {
            move_loc+=1;
            counter+=1;
            path_moves[i][LEFT] += piece_pos << counter;
        }

        // Get top left moves
        move_loc = i; // index from top
        counter = 0;
        while ((move_loc%8 < 7) && (move_loc/8)<7) {
            move_loc += 9; 
            counter += 9;
            path_moves[i][TOP_LEFT] += piece_pos << counter;
        }
    }
}

template<class _container, class _Ty>
inline bool in_list(_container _C, const _Ty& _Val) {
    return std::find(_C.begin(), _C.end(), _Val) == _C.end();
}

int main(int argc, char** argv) {
    size_t pos = 0;
    bool gen_all = false; 
    enum Piece { PATH, PAWN, KING, KNIGHT } piece;
    std::array<std::string, 4> piece_list = {"path", "pawn", "king", "knight"};

    if (argc == 4) {
        if (std::string(argv[1]) == "for") {
            if (std::string(argv[2]) == "all") {
                gen_all = true;
            } else {
                gen_all = false;
                try {
                    pos = std::stoi(argv[2]); 
                } catch (std::invalid_argument) {
                    std::cerr << "ERROR: non-numeric 2nd argument supplied\n";
                    return -1;
                }
            }
        }
        bool found = false;
        for (u8 i=0; i<piece_list.size(); i++) {
            if (std::string(argv[3]) == piece_list[i]) {
                piece = (Piece)i;
                found = true;
                break;
            }
        }

        if (!found) {
            std::cerr << "ERROR: " << argv[3] << " is not a valid piece\n";
            std::cerr << "valid choices are 'path', 'pawn', 'king', and 'knight'\n";
            return -1;
        }

    } else {
        std::cerr << "Incorrect amount of arguments\n" 
                  << "Usage: ./gen_path_moves for 0 pawn\n"
                  << "    Or: ./gen_path_moves for all path\n";
    }

    switch (piece) {
        case PATH: {
            gen_moves_path();

            std::array<std::string, 8> directions = { 
               "TOP", "TOP_RIGHT", "RIGHT", "BOTTOM_RIGHT", 
               "BOTTOM", "BOTTOM_LEFT", "LEFT", "TOP_LEFT"
            };

            if (!gen_all) {
                for (size_t i=0; i < path_moves[pos].size(); i++) {
                    std::cout << directions[i] << "\n";
                    print_piece_map(path_moves[pos][i]);
                }
            } else {
                std::cout << "std::array<std::array<u64, 8>, 64> path_moves = {\n";
                for (int i =0; i<path_moves.size(); i++) {
                    std::cout << "    {\n";
                    for (int j=0; j<path_moves[i].size(); j++) {
                        std::cout << "        0x" << std::hex << std::uppercase 
                                  << std::setfill('0') << std::setw(16)
                                  << path_moves[i][j];

                        if (j != path_moves[i].size()-1) std::cout << ",";
                        std::cout << "\n";
                    }
                    std::cout << "    }";
                    if (i != path_moves.size()-1) std::cout << ",";
                    std::cout << "\n";
                }
                std::cout << "};\n";
            }
            break;
        }
        case PAWN: {
            gen_moves_pawn();
            if (!gen_all) {
                std::array<std::string, 4> pawn_opts = { 
                    "WHITE_MOVE", "WHITE_ATTACK", 
                    "BLACK_MOVE", "BLACK_ATTACK"
                };

                for (int i=0; i < pawn_moves[pos].size(); i++) {
                    std::cout << "\n" << pawn_opts[i] << "\n";
                    print_piece_map(pawn_moves[pos][i]);
                }
            } else {
                std::cout << "std::array<std::array<u64,4>, 64> " 
                          << "pawn_moves = {{\n"; 
                for (int i=0; i<pawn_moves.size(); i++) {
                    std::cout << "    {\n";
                    for (int j=0; j<pawn_moves[i].size(); j++) {
                        std::cout << "        0x" << std::hex
                                  << std::uppercase << std::setfill('0')
                                  << std::setw(16)
                                  << pawn_moves[i][j];
                        if (j!=pawn_moves[i].size()-1) std::cout << ",";
                        std::cout << "\n";
                    }
                    std::cout << "    }";

                    if (i != pawn_moves.size()-1) std::cout << ",";
                    std::cout << "\n";
                }
                std::cout << "}};\n";
            }
            
            break;
        }
        case KING: {
            gen_moves_king();
            if (!gen_all) {
                print_piece_map(king_moves[pos]);
            } else {
                std::cout << "std::array<u64, 64> king_moves = {\n"; 
                for (int i=0; i< king_moves.size(); i++) {
                    std::cout << "    0x" << std::hex << std::uppercase
                              << std::setfill('0') << std::setw(16)
                              << king_moves[i];
                    if (i != knight_moves.size() - 1) std::cout <<",";
                    std::cout << "\n";
                }
                std::cout << "};\n";
            }
            break;
        }
        case KNIGHT: {
            gen_moves_knight();
            if (!gen_all) {
                print_piece_map(knight_moves[pos]);
            } else {
                std::cout << "std::array<std::array<u64,4>, 64> " 
                          << "pawn_moves = {{\n"; 
                for (int i=0; i< knight_moves.size(); i++) {
                    std::cout << "    0x" << std::hex << std::uppercase
                              << std::setfill('0') << std::setw(16)
                              << knight_moves[i];
                    if (i != knight_moves.size() - 1) std::cout <<",";
                    std::cout << "\n";
                }
                std::cout << "}};\n";
            }
            break;
        }
        default: {
            std::cerr << "ERROR: piece selection incorrect\n";
            return -1;
            break;
        }
    }
}
