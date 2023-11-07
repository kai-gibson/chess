#include <iostream>
#include <string>
#include <algorithm>
#include <chrono>
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

u64 get_path_moves(u64 piece_pos, bool is_white, u8 direction) {
    u64 moves = 0;
    for (u8 i=direction; i<8; i+=2) { // for diagonals
        // get direction & with all pieces
        u64 barriers = arrays.path_moves[piece_pos][i] 
            & (arrays.pieces[WHITE] | arrays.pieces[BLACK]);

        // if bottom direction get MSB, else get LSB
        u8 barrier_pos = (in_list(arrays.MSB, i) 
                ? 63 - __builtin_clzl(barriers) 
                : __builtin_ctzl(barriers));

        // get the same direction from the LSB/MSB in barriers
        // get resulting possible moves 
        moves += arrays.path_moves[piece_pos][i] & 
            ~arrays.path_moves[barrier_pos][i];
    }

    // filter so you can't take your own pieces
    moves = moves & 
        ~(is_white ? arrays.pieces[WHITE] : arrays.pieces[BLACK]);

    return moves;
}

u64 get_knight_moves(u8 piece_pos, bool is_white) {
    u64 moves;

    // can't move if blocked by own side
    //std::cout << "knight possible moves:\n";
    //print_piece_map(arrays.knight_moves[piece_pos]);

    moves = arrays.knight_moves[piece_pos]
        & ~arrays.pieces[is_white ? WHITE : BLACK];
    //std::cout << "knight unobstructed moves:\n";
    //print_piece_map(moves);

    return moves;
}

u64 get_pawn_moves(u8 piece_pos, bool is_white) {
    u64 moves = 0;

    if (is_white) {
        // can't move if a piece blocks
        moves |= arrays.pawn_moves[piece_pos][WHITE_MOVE] 
            & ~(arrays.pieces[WHITE] | arrays.pieces[BLACK]);

        // can only take if enemy in attack zone
        moves |= arrays.pawn_moves[piece_pos][WHITE_ATTACK]
            & arrays.pieces[BLACK];
    } else {
        // can't move if a piece blocks
        moves |= arrays.pawn_moves[piece_pos][BLACK_MOVE] 
            & ~(arrays.pieces[WHITE] | arrays.pieces[BLACK]);

        // can only take if enemy in attack zone
        moves |= arrays.pawn_moves[piece_pos][BLACK_ATTACK]
            & arrays.pieces[WHITE];
    }

    // but en passant can only be done the turn after a dbl move...
    // check en passant
    return moves;
}

u64 get_king_moves(u8 piece_pos, bool is_white) {
    u64 moves;

    // can't move if blocked by own side
    moves = arrays.king_moves[piece_pos]
        & ~arrays.pieces[is_white ? WHITE : BLACK];

    return moves;
}

i8 map_piece(u64 piece) {
    // check which piece it is
    for (u8 j=2; j<8; j++) {
        if (piece & arrays.pieces[j]) return j;
    }

    return -1;
}

void gen_moves(std::array<u64, 8> board) {
    u64 piece;
    u64 moves;

    // loop from H1 to A8
    for (u8 i = 0; i<64; i++) {
        // get bitboard for current piece
        bool is_white;
        piece = get_bin_num(i);
        moves = 0;

        // check team
        is_white = piece & board[WHITE];

        if (!is_white && !(piece & board[BLACK])) { // no piece
            arrays.potential_moves[i] = 0;
            continue;
        } 

        switch(map_piece(piece)) {
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

u64 moves_for(u8 start, u8 end) {
    u64 moves = 0;
    for (int i=start; i<end; i++) {
        moves |= arrays.potential_moves[i];
    }
    return moves;
}

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

int main(int argc, char** argv) {
    if (argc==2) {
        if (std::string(argv[1]) == "-test") {
            std::cout << "running unit tests\n";
            run_tests();
        }
    }

    auto start = std::chrono::high_resolution_clock::now();

    gen_moves(arrays.pieces);

    auto end = std::chrono::high_resolution_clock::now();

    {
        using namespace std::chrono;
        std::cout << "Took: " 
                  << duration<double, std::milli>(end-start).count()
                  << "ms\n";
    }


    print_piece_map(moves_for(0,63+1));

    /*
    TODO:
        [] - Handle En Passant
        [] - Handle Castling
        [] - check for check
               - if in check can't move without leaving check
        [] - check for checkmate
               - do I wanna gen all moves that could break check?
        [] - Make func to actually move pieces
    */
}
