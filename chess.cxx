#include <iostream>
#include <string>
#include <algorithm>
#include "chess.h"
#include "print_board.h"


// Arrays object
Arrays arrays;

// convert an array index into a binary number with n trailing 0's
// e.g. 7 = 0b1000 0000
inline u64 get_bin_num(i8 arr_indx) { 
    return u64(1) << arr_indx;
}

template<class _container, class _Ty>
inline bool in_list(_container _C, const _Ty& _Val) {
    return std::find(_C.begin(), _C.end(), _Val) != _C.end();
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


u64 get_path_moves(u64 piece_pos, bool is_white, u8 direction) {
    u64 moves = 0;
    for (u8 i=direction; i<8; i+=2) { // for diagonals
        // get direction & with all pieces
        u64 barriers = arrays.path_moves[piece_pos][i] 
            & (arrays.pieces[WHITE] | arrays.pieces[BLACK]);

        // if top direction get LSB, else get MSB
        u8 barrier_pos = (in_list(arrays.MSB, i) ? 
             63 - __builtin_clzl(barriers) :
            __builtin_ctzl(barriers));

        // get the same direction from the LSB/MSB in barriers
        // get resulting possible moves 
        moves += arrays.path_moves[piece_pos][i] & 
            ~arrays.path_moves[barrier_pos][TOP];
    }

    // filter so you can't take your own pieces
    moves = moves & 
        ~(is_white ? arrays.pieces[WHITE] : arrays.pieces[BLACK]);

    return moves;
}

u64 get_knight_moves(u8 piece_pos, bool is_white) {
    (void)is_white;
    (void)piece_pos;
    return 0;
}

u64 get_pawn_moves(u8 piece_pos, bool is_white) {
    (void)is_white;
    (void)piece_pos;
    return 0;
}

u64 get_king_moves(u8 piece_pos, bool is_white) {
    (void)is_white;
    (void)piece_pos;
    return 0;
}

void gen_moves() {
    u64 piece;
    bool is_white;
    u64 moves;

    // loop from H1 to A8
    for (u8 i = 0; i<64; i++) {
        // get bitboard for current piece
        piece = get_bin_num(i);
        moves = 0;

        // check team
        if (piece & arrays.pieces[WHITE]) is_white = true;

        // check which piece it is
        for (u8 j=2; j<8; j++) {
            if (!(piece & arrays.pieces[j])) break;
            switch (j) {
                case PAWNS:
                    moves = get_pawn_moves(i, is_white);
                    break;
                case ROOKS:
                    moves = get_path_moves(i, is_white, opt::STRAIGHT);
                    break;
                case KNIGHTS:
                    moves = get_knight_moves(i, is_white);
                    break;
                case BISHOPS:
                    moves = get_path_moves(i, is_white, opt::DIAG);
                    break;
                case QUEENS:
                    moves = get_path_moves(i, is_white, opt::STRAIGHT) 
                        & get_path_moves(i, is_white, opt::DIAG);
                    break;
                case KINGS:
                    moves = get_king_moves(i, is_white);
                    break;
                default:
                    break;
            }
            arrays.potential_moves[i] = moves;
        }
    }
}

int main(int argc, char** argv) {
    if (argc==2) {
        if (std::string(argv[1]) == "-test") {
            std::cout << "running unit tests\n";
            run_tests();
        }
    }

    print_piece_map(arrays.pawn_moves[55][BLACK_ATTACK]);

    /*
    TODO:
        [] - KING moves 
        [] - PAWN moves
        [] - KNIGHT moves

    For knights and kings I'll have an array of their possible moves 
    for each tile they could be on
    for pawns I can still do that but I also need to evaluate if they
    can take a piece on their top right & left, as well as check for 
    en passant
    */
}
