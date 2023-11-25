#include "print_board.h"
#include <array>
#include <string>
#include <iostream>
#include <bitset>
#include <thread>
#include <chrono>

enum { WHITE, BLACK, PAWNS, ROOKS, // pieces
       KNIGHTS, BISHOPS, QUEENS, KINGS }; 


void print_piece_map(unsigned long board) {
    std::array<char, 8> rows = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
    std::array<char, 8> cols = {'8', '7', '6', '5', '4', '3', '2', '1'};

    auto print_rows = [&]{
        std::cout << "\n   ";
        for (char ch : rows) {
            std::cout << ch << " ";
        }
        std::cout << "\n";
    };

    std::string str_board = std::bitset<64>(board).to_string() ;

    print_rows();
    std::cout <<'\n';

    std::string row;
    int count = 0;
    for (int i = 0; i < int(str_board.length()); i++) {
        count++;
        if (i==0) row+=' ';
        row += str_board[i];
        row += ' ';

        if (count % 8==0) {
            std::cout << cols[count/8-1] << " " << row 
                      << " " << cols[count/8-1] << "\n";
            row = ' ';
        } 
    }

    print_rows();
}

#define NC "\x1b[0m"
#define WHITE "\x1b[30;47m"
#define BLACK "\x1b[37;40m"

void pretty_print_board(std::array<unsigned long, 8> pieces) {
    std::array<char, 8> rows = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
    std::array<char, 8> cols = {'8', '7', '6', '5', '4', '3', '2', '1'};
    std::array<char, 8> piece_chars = {0, 0, 'P', 'R', 'H', 'B', 'Q', 'K'};

    auto print_rows = [&]{
        std::cout << "\n    ";
        for (char ch : rows) {
            std::cout << ch << "   ";
        }
        std::cout << "\n";
    };

    std::array<std::string, 64> board;
    std::string white_pieces = std::bitset<64>(pieces.at(0)).to_string();
    std::string black_pieces = std::bitset<64>(pieces.at(1)).to_string();

    for (size_t i=0; i<white_pieces.size(); i++) {
        if (white_pieces.at(i) == '1') {
            board.at(i) += WHITE;
        }
        else if (black_pieces.at(i) == '1') {
            board.at(i) += BLACK;
        } 
    }

    for (size_t i=2; i<8; i++) {
        std::string curr_piece = std::bitset<64>(pieces.at(i)).to_string();
        for (size_t j=0; j<curr_piece.size(); j++) {
            if (curr_piece.at(j) == '1') {
                board.at(j) += piece_chars.at(i);
                board.at(j) += NC;
            }
        }
    }

    for (size_t i =0; i<board.size(); i++) {
        if (board.at(i) == "") {
            board.at(i) = " ";
        }
    }

    print_rows();
    std::cout << "  |---|---|---|---|---|---|---|---|\n";
    std::string row;
    int count =0;

    for (size_t i=0; i<board.size(); i++) {
        count++;

        row += "| "; 
        row += board.at(i);
        row += " ";
        
        if (count % 8==0) {
            std::cout << cols.at(count/8-1) << " " << row
                      << "| " << cols.at(count/8-1) << "\n";
            std::cout << "  |---|---|---|---|---|---|---|---|\n";
            row = "";

        }

    }
    print_rows();
}
