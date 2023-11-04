#include <iostream>
#include <string>
#include "chess.h"
#include "print_board.h"

// Arrays object
Arrays arrays;

// convert an array index into a binary number with n trailing 0's
// e.g. 7 = 0b1000 0000
u64 get_bin_num(i8 arr_indx) { 
    return u64(1) << arr_indx;
}

//u64 shift(char direction, u64 bin_val, u8 amt, u32 opts) {
//    // OPTS:
//    //   MOVE_ONE_ROW -- only move one row, will ret 0 if more than 1 row
//    //   MOVE_TWO_ROWS -- only move 2 rows, no more or less
//    //   MOVE_ANY_ROWS -- move any rows
//    //   default -- don't move rows, only cols
//
//    if (opts & MOVE_ONE_ROW) {
//        u8 index = __builtin_ctzl(bin_val);
//
//
//    }
//    else if (opts & MOVE_TWO_ROW) {
//    }
//    else if (opts & MOVE_ANY_ROW) {
//
//    } else {
//    }
//    
//
//    return 0;
//}


// debugging functions

// unit tests
#define EQUALS(x,y) { if (x != y) std::cout << __FUNCTION__ \
    << " failed on line " << __LINE__ << std::endl \
    << x << " does not equal " << y << "\n" ; }

void run_tests() {
    EQUALS(__builtin_ctzl(0x8000000000000000), 63);
    EQUALS(__builtin_ctzl(0x0000000000000001), 0);
    EQUALS(__builtin_ctzl(0x0000001000000000), 36);

    EQUALS(get_bin_num(63), 0x8000000000000000);
    EQUALS(get_bin_num(0), 0x0000000000000001);
}

u64 gen_moves_rook(u64 piece, u64 obstacles) {
    u8 piece_pos = __builtin_ctzl(piece);

    u8 row = piece_pos / 8;
    u8 col = piece_pos % 8;

    u64 all_moves = arrays.cols[col] ^ arrays.rows[row];

    print_piece_map(all_moves);
    print_piece_map(obstacles);
    
    //for (int i = 7; i>=0; i--) {
    //}

    return all_moves;
}

u64 get_diag_moves(u64 piece_pos) {
    return 0;
}

u64 get_line_moves(u64 piece_pos) {
    return 0;
}

void gen_moves() {
    u64 piece;
    bool is_white;
    for (u8 i = 0; i<64; i++) {
        piece = get_bin_num(i);
        if (piece & arrays.pieces[WHITE]) is_white = true;


        
    }
}

int main(int argc, char** argv) {
    if (argc==2) {
        if (std::string(argv[1]) == "-test") {
            std::cout << "running unit tests\n";
            run_tests();
        }
    }
}
