#include <iostream>
#include <array>
#include <vector>
#include <bitset>
#include <cstring>

using u8 = unsigned char;
using i8 = char;
using u64 = unsigned long;

enum {WHITE, BLACK, PAWNS, ROOKS, KNIGHTS, BISHOPS, QUEENS, KINGS};
std::array<u64, 8> pieces = {
    0x000000000000FFFF, // WHITE
    0xFFFF000000000000, // BLACK
    0x00FF00000000FF00, // PAWNS
    0x8100000000000081, // ROOKS
    0x4200000000000042, // KNIGHTS
    0x2400000000000024, // BISHOPS
    0x1000000000000010, // QUEENS
    0x0800000000000008, // KINGS
};

std::array<u64, 64> potential_moves;

// convert a binary number to an array index
// e.g. 0b1000 0000 = 7
i8 get_arr_pos(u64 binary_val) { 
    for (int i=0; i<64; i++) {
        if (binary_val >> i == 1) return i;
    }
    return -1;
}


// convert an array index into a binary number
// e.g. 7 = 0b1000 0000
u64 get_bin_num(i8 arr_indx) { 
    return u64(1) << arr_indx;
}

// debugging functions

// unit tests
#define EQUALS(x,y) { if (x != y) std::cout << __FUNCTION__ \
    << " failed on line " << __LINE__ << std::endl \
    << x << " does not equal " << y << "\n" ; }

template<typename T, typename Tn>
bool equals(T val_one, Tn val_two) {
    if (val_one != val_two) { 
        std::cout << __FUNCTION__ 
        << " failed on line " << __LINE__ << std::endl 
        << val_one << " does not equal " << val_two << "\n" ;
        return false;
    }
    return true;
}

void run_tests() {
    EQUALS(get_arr_pos(0x8000000000000000), 63);
    EQUALS(get_arr_pos(0x0000000000000001), 0);

    EQUALS(get_bin_num(62), 0x8000000000000000);
    EQUALS(get_bin_num(0), 0x0000000000000001);
}

void print_piece_map(u64 board) {
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
            std::cout << cols[count/8-1] << " " << row << "\n";
            row = ' ';
        } 
    }

    print_rows();
}



int main(int argc, char** argv) {
    if (argc==2) {
        if (!std::strcmp(argv[1], "-test")) {
            std::cout << "running unit tests\n";
            run_tests();
        }
    }
}
