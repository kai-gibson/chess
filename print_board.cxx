#include "print_board.h"
#include <array>
#include <string>
#include <iostream>
#include <bitset>

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
