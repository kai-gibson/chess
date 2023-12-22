#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <chrono> 
#include <thread>

#include "../move_generation.h"
#include "print_board.h"

// unit tests
#define EQUALS(x,y) { if (x != y) std::cout << __FUNCTION__ \
    << " failed on line " << __LINE__ << std::endl \
    << x << " does not equal " << y << "\n" ; }

void run_tests() {
    const Arrays arrays;
    const Pieces test_bishop = {
        0x000000000010EFFF, // WHITE
        0xFFFF000000000000, // BLACK
        0x00FF00000010EF00, // PAWNS
        0x8100000000000081, // ROOKS
        0x4200000000000042, // KNIGHTS
        0x2400000000000024, // BISHOPS
        0x1000000000000010, // QUEENS
        0x0800000000000008, // KINGS
    };
    EQUALS(CTZ(0x8000000000000000), 63);
    EQUALS(CTZ(0x0000000000000001), 0);
    EQUALS(CTZ(0x0000001000000000), 36);

    EQUALS(get_bin_num(63), 0x8000000000000000);
    EQUALS(get_bin_num(0), 0x0000000000000001);

    auto start = std::chrono::high_resolution_clock::now();
    Moves m = gen_moves(arrays.initial_positions);
    auto end = std::chrono::high_resolution_clock::now();

    EQUALS(moves_for(m, 8,16), 0xFFFF0000);
    EQUALS(moves_for(m, 0,8), 0xA50000);
    EQUALS(moves_for(m, 48,56), 0x0000FFFF00000000);
    EQUALS(moves_for(m, 56,64), 0x0000A50000000000);

    m = gen_moves(test_bishop);
    pretty_print_board(test_bishop);
    EQUALS(moves_for(m, m.board.bishops), 0x0000010204081000);

    { using namespace std::chrono;
        std::cout << "Gen moves took: " 
                  << duration<double, std::milli>(end-start).count()
                  << "ms\n";
    }

    EQUALS(str_to_index("A3"), 23);
    EQUALS(str_to_index("E4"), 27);

    EQUALS(index_to_str(63), "A8");
    EQUALS(index_to_str(0), "H1");
    EQUALS(index_to_str(27), "E4");
    EQUALS(index_to_str(23), "A3");
}

int main(int argc, char** argv) {
    if (argc == 2) {
        if (std::string(argv[1]) == "-test") {
            std::cout << "running unit tests\n";
            run_tests();
        }
    }

    std::string input;
    bool white_turn = true;
    size_t space_pos;
    std::string start;
    std::string end;
    std::stringstream error;

    auto error_out = [&]() { 
        error.str("");
        error.clear();
        error << "\n" << input << " is incorrect format," 
            << " expected something like 'A2 B3'\n";
    };

    Arrays arrays;
    Moves m = gen_moves(arrays.initial_positions);

    while (true) {
        for (int i=0; i<64; i++) {
            std::cout << "                                         "
                      << "                                         \n";
        }
        std::cout << "\033[0;64H"; // refresh the screen

        if (error.str() != "") std::cout << error.str();

        pretty_print_board(m.board.pieces);

        std::cout << (white_turn 
                ? "White team's turn\n" 
                : "Black team's turn\n");

        bool check = king_in_danger(m, white_turn);

        if (check && moves_for(m, (white_turn 
                    ? m.board.white_pieces 
                    : m.board.black_pieces)) == 0) {

            std::cout << "Checkmate!! " 
                      << (white_turn 
                            ? "White" 
                            : "Black") 
                      << " team loses!\n";
            break;
        }

        if (check) {
            std::cout << "Your king is in check!\n";
        }

        std::cout << "\nEnter move: \n";
        std::getline(std::cin, input);

        if (input.size() != 5) {
            error_out();
            continue;
        }

        space_pos = input.find(' ');

        if (space_pos == std::string::npos) {
            error_out();
            continue;
        }

        start = input.substr(0, space_pos);
        if (start.size() != 2) { 
            error_out();
            continue; // try again
        }

        end = input.substr(space_pos+1, 2);

        Move_Piece mp = move_piece(m, str_to_index(start), 
                                   str_to_index(end), white_turn);

        if (!mp.res) continue;

        error.str("");
        error.clear();
        m = mp.moves;

        white_turn = !white_turn; // successful turn, piece moved
    }
}
