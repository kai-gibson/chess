#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <chrono> 
#include <thread>
#include "chess.h"
#include "print_board.h"

const Arrays arrays; // array constants
// Util functions
u64 moves_for(Moves m, std::vector<u8> v) {
    u64 moves = 0;
    for (i8 i : v) {
        moves |= m.potential_moves.at(i);
    }
    return moves;
}

u64 moves_for(Moves m, i8 start, i8 end) {
    u64 moves = 0;
    for (i8 i = start; i<end; i++) {
        moves |= m.potential_moves.at(i);
    }
    return moves;
}

bool king_in_danger(Moves m, bool is_white) {
    if (moves_for(m, is_white ? m.board.black_pieces 
                : m.board.white_pieces) 
        & (m.board.pieces.at(KINGS) 
            & m.board.pieces.at(is_white ? WHITE : BLACK))) {
        return true;
    }
    return false;
}

std::string index_to_str(u8 index) {
    std::string ret_val;
    if (index <= 63) {
        i8 col = index % 8;
        i8 row = index / 8;
        
        ret_val += ('H' - col); 
        ret_val += ('1' + row); 

        return ret_val;
    } else { 
        return "";
    }
}

i8 str_to_index(std::string loc) {
    // H1 becomes 0, A8 becomes 63 
    // bad params return -1
    i8 row;
    i8 col;
    i8 indx;

    if (loc.at(0) >= 'A' && loc.at(0) <= 'H') {
        col = abs(loc.at(0) - 'H');
    } else {
        std::cerr << "Error: '" << loc << " is an invalid tile, values "
                  << "must between A1 and H8\n";
        return -1;
    }

    if (loc.at(1) >= '1' && loc.at(1) <= '8') {
        row = loc.at(1) - '1';
    } else {
        std::cerr << "Error: '" << loc << " is an invalid tile, values "
                  << "must between A1 and H8\n";
        return -1;
    }

    indx = col + (8*row);
    
    return indx;
}

// convert an array index into a binary number with n trailing 0's e.g. 7 = 0b1000 0000
inline u64 get_bin_num(i8 arr_indx) { 
    return u64(1) << arr_indx;
}

template<class _container, class _Ty>
inline bool in_list(_container _C, const _Ty& _Val) {
    return std::find(_C.begin(), _C.end(), _Val) != _C.end();
}

i8 map_piece(Pieces p, u64 piece) {
    // check which piece it is
    for (u8 j=2; j<8; j++) {
        if (piece & p.at(j)) return j;
    }

    return -1;
}

Move_Piece move_piece(Moves m, i8 start, i8 end, bool is_white, bool
        gen=true) {
    if (!in_list((is_white 
                    ? m.board.white_pieces 
                    : m.board.black_pieces), 
                start)) {

        std::cerr << "Error, no " << (is_white ? "white " : "black ") 
                  << "piece at " << index_to_str(start) << "\n";

        return { false, m }; // false means move couldn't be made
    }

    u64 end_bin = get_bin_num(end);
    u64 start_bin = get_bin_num(start);

    if (!(end_bin & m.potential_moves.at(start))) {
        std::cerr << "Error, cannot move piece " << index_to_str(start) << " to " 
                  << index_to_str(end) << "\n";
        return { false, m };
    }
    
    // move is legal
    i8 piece_type = map_piece(m.board.pieces, start_bin);
    i8 team = is_white ? WHITE : BLACK;

    // remove start pos
    m.board.pieces.at(piece_type) = 
        m.board.pieces.at(piece_type) ^ start_bin;

    m.board.pieces.at(team) = 
        m.board.pieces.at(team) ^ start_bin;

    i8 enemy_team = is_white ? BLACK : WHITE;

    // if enemy at end pos, remove it
    if (m.board.pieces.at(enemy_team) & end_bin) {
        i8 enemy_piece_type = map_piece(m.board.pieces, end_bin);

        m.board.pieces.at(enemy_piece_type) = 
            m.board.pieces.at(enemy_piece_type) ^ end_bin;

        m.board.pieces.at(enemy_team) = 
            m.board.pieces.at(enemy_team) ^ end_bin;
    } 

    // take piece if en passant
    else if (piece_type == PAWNS && (m.board.pieces.at(enemy_team) 
        & (is_white ? (end_bin >> 8) : (end_bin << 8)) 
        & (m.board.pieces.at(PAWNS)))) {

        // clear the piece
        m.board.pieces.at(PAWNS) = 
            m.board.pieces.at(PAWNS) ^ (is_white 
                    ? (end_bin >> 8) 
                    : (end_bin << 8));

        // clear from team
        m.board.pieces.at(enemy_team) = 
            m.board.pieces.at(enemy_team) ^ (is_white 
                    ? (end_bin >> 8) 
                    : (end_bin << 8));      
    }

    // handle promotion
    if (piece_type == PAWNS && (end / 8 == (is_white ? 7 : 0))) {
        piece_type = QUEENS;
    } 

    // handle castling
    if (piece_type == KINGS && start == (is_white ? 3 : 59)
            && (abs(end - start) == 2)) {

        i8 rook_pos = (is_white ? 0 : 56);
        i8 rook_end_pos = (is_white ? 0 : 56);

        switch(end % 8) {
            case 5:
                rook_pos += 7;
                rook_end_pos += 4;
                break;
            case 1:
                rook_end_pos += 2;
                break;
            default:
                throw std::runtime_error("FUCK!");
                break;
        }

        m.board.pieces.at(ROOKS) = 
            m.board.pieces.at(ROOKS) ^ get_bin_num(rook_pos);

        m.board.pieces.at(team) = 
            m.board.pieces.at(team) ^ get_bin_num(rook_pos);

        m.board.pieces.at(ROOKS) = 
            m.board.pieces.at(ROOKS) | get_bin_num(rook_end_pos);

        m.board.pieces.at(team) = 
            m.board.pieces.at(team) | get_bin_num(rook_end_pos);


    }

    // add end pos
    m.board.pieces.at(piece_type) = 
        m.board.pieces.at(piece_type) | end_bin;

    m.board.pieces.at(team) = 
        m.board.pieces.at(team) | end_bin;


    if (gen) {
        // generate next turn's moves
        m = gen_moves(m.board.pieces, false);

        // check en_passant
        // if pawn just moved 2 squares
        if (piece_type == PAWNS && (start / 8 == (is_white ? 1 : 6) 
                    && end / 8 == (is_white ? 3 : 4))) {
            Piece_List pawns_ep;

            // if space on right 
            if (end % 8 >= 0) {
                u64 right = end_bin >> 1;
                if (m.board.pieces.at(is_white ? BLACK : WHITE) & right ) {
                    pawns_ep.push_back(__builtin_ctzl(right));
                }
            }

            // if space on left
            if (end % 8 <= 7) {
                u64 left = end_bin << 1;
                if ((m.board.pieces.at(is_white ? BLACK : WHITE)
                            & m.board.pieces.at(PAWNS)) & left) {

                    pawns_ep.push_back(__builtin_ctzl(left));
                }
            }

            // add move
            for (u8 i : pawns_ep) {
                m.potential_moves.at(i) |= (is_white 
                        ? (end_bin >> 8)
                        : (end_bin << 8));
            }

        }

        // check castling

        auto check_castling = [](Moves m, bool is_white, bool kingside)-> Moves {
            i8 king_pos = __builtin_ctzl(m.board.pieces.at(KINGS) 
                                     & m.board.pieces.at(is_white 
                                         ? WHITE : BLACK));; 

            i8 king_origin = is_white ? 3 : 59;
            u64 rook_origin;
            u8 side;
            u64 move;
            u8 team = is_white ? WHITE : BLACK;

            if (is_white) {
                side = kingside ? WHITE_KINGSIDE : WHITE_QUEENSIDE;
                move = kingside ? 0x02 : 0x20;
            } else {
                side = kingside ? BLACK_KINGSIDE : BLACK_QUEENSIDE;
                move = kingside ? 0x0200000000000000 : 0x2000000000000000;
            }

            if (kingside) {
                rook_origin = is_white ? 0x01 : 0x0100000000000000;
            } else {
                rook_origin = is_white ? 0x80 : 0x8000000000000000;
            }

            if (king_pos == king_origin
                && (m.board.pieces.at(ROOKS) 
                    & m.board.pieces.at(team) 
                        & rook_origin)) {

                // if path unobstructed
                if (!(arrays.castling.at(side) &
                    (m.board.pieces.at(WHITE) |
                    m.board.pieces.at(BLACK)))) {
                    // add move
                    m.potential_moves.at(king_pos) |= move;
                }
            }
            return m;
        };

        //check_castling(true, true);
        m = check_castling(m, enemy_team, true);
        m = check_castling(m, enemy_team, false);
        m = check_castling(m, team, true);
        m = check_castling(m, team, false);
        //check_castling(true, false);
        //check_castling(false, true);
        //check_castling(false, false);

        //// check kingside castling
        //if (king_pos == (is_white ? 3 : 59) 
        //    && (m.board.pieces.at(ROOKS) 
        //        & m.board.pieces.at(team) 
        //            & (is_white ? 0x01 : 0x0100000000000000))) {

        //    // if path unobstructed
        //    if (!(arrays.castling.at(is_white 
        //                    ? WHITE_KINGSIDE 
        //                    : BLACK_KINGSIDE) &
        //                (m.board.pieces.at(WHITE) |
        //                m.board.pieces.at(BLACK)))) {

        //        // add move
        //        m.potential_moves.at(king_pos) |= (is_white 
        //                ? 0x02 
        //                : 0x0200000000000000);
        //    }
        //}

        //// check queenside castling
        //if (king_pos == (is_white ? 3 : 59) 
        //    && (m.board.pieces.at(ROOKS) 
        //        & m.board.pieces.at(team) 
        //            & (is_white ? 0x80 : 0x8000000000000000))) {

        //    // if path unobstructed
        //    if (!(arrays.castling.at(is_white 
        //                    ? WHITE_QUEENSIDE 
        //                    : BLACK_QUEENSIDE) &
        //                (m.board.pieces.at(WHITE) |
        //                m.board.pieces.at(BLACK)))) {

        //        // add move
        //        m.potential_moves.at(king_pos) |= (is_white 
        //                ? 0x20 
        //                : 0x2000000000000000);
        //    }
        //}

        // run filter to check if newly gen'd moves are legal
        m = filter(m, m.board.white_pieces, true);
        m = filter(m, m.board.black_pieces, false);
    }

    return { true, m };
}

// Move generation by piece
u64 get_path_moves(Pieces p, u64 piece_pos, bool is_white, u8 direction) {
    u64 moves = 0;
    for (u8 i=direction; i<8; i+=2) { // for diagonals
        // get direction & with all pieces
        u64 barriers = arrays.path_moves.at(piece_pos).at(i) 
            & (p.at(WHITE) | p.at(BLACK));


        u8 barrier_pos = 0;
        // if bottom direction get MSB, else get LSB
        if (barriers) {
            barrier_pos = (in_list(arrays.MSB, i) 
                    ? 63 - __builtin_clzl(barriers) 
                    : __builtin_ctzl(barriers));
        } 

        // get the same direction from the LSB/MSB in barriers
        // get resulting possible moves 
        if (barrier_pos) {
            moves += arrays.path_moves.at(piece_pos).at(i) & 
                ~arrays.path_moves.at(barrier_pos).at(i);
        } else {
            moves += arrays.path_moves.at(piece_pos).at(i);
        }
    }

    // filter so you can't take your own pieces
    moves = moves & 
        ~(is_white ? p.at(WHITE) : p.at(BLACK));

    return moves;
}

u64 get_knight_moves(Pieces p, u8 piece_pos, bool is_white) {
    u64 moves;

    // can't move if blocked by own side
    //std::cout << "knight possible moves:\n";
    //print_piece_map(arrays.knight_moves[piece_pos]);

    moves = arrays.knight_moves.at(piece_pos)
        & ~p.at(is_white ? WHITE : BLACK);
    //std::cout << "knight unobstructed moves:\n";
    //print_piece_map(moves);

    return moves;
}

u64 get_pawn_moves(Pieces p, u8 piece_pos, bool is_white) {
    u64 moves = 0;

    if (is_white) {
        // can't move if a piece blocks
        moves |= arrays.pawn_moves.at(piece_pos).at(WHITE_MOVE)
            & ~(p.at(WHITE) | p.at(BLACK));

        // can only take if enemy in attack zone
        moves |= arrays.pawn_moves.at(piece_pos).at(WHITE_ATTACK)
            & p.at(BLACK);
    } else {
        // can't move if a piece blocks
        moves |= arrays.pawn_moves.at(piece_pos).at(BLACK_MOVE) 
            & ~(p.at(WHITE) | p.at(BLACK));

        // can only take if enemy in attack zone
        moves |= arrays.pawn_moves.at(piece_pos).at(BLACK_ATTACK)
            & p.at(WHITE);
    }

    // but en passant can only be done the turn after a dbl move...
    // check en passant

    return moves;
}

u64 get_king_moves(Pieces p, u8 piece_pos, bool is_white) {
    u64 moves;

    // can't move if blocked by own side
    moves = arrays.king_moves.at(piece_pos)
        & ~p.at(is_white ? WHITE : BLACK);

    return moves;
}

Moves filter(Moves m, Piece_List pl, bool is_white) {
    for (size_t i=0; i < pl.size(); i++) {
        // get all moves for piece
        i8 piece_pos = pl.at(i);
        u64 moves = m.potential_moves.at(piece_pos); 

        i8 curr_move = 0;
        while (moves != 0) {
            // get position of just this move
            curr_move = __builtin_ctzl(moves); 
            Move_Piece mp = move_piece(m, piece_pos, curr_move, is_white, false);

            if (!mp.res) { 
                throw std::runtime_error("You really fucked up\n");
            }

            Moves future = gen_moves(mp.moves.board.pieces, false);

            // If king is in danger
            if (king_in_danger(future, is_white)) {
                // remove move from potentials
                m.potential_moves.at(piece_pos) ^= get_bin_num(curr_move);
            }
        
            moves ^= get_bin_num(curr_move); // remove from list
        }
    }
    return m;
};

// Move generation for board
Moves gen_moves(Pieces pieces, bool filter_legal=true) {
    Moves m;
    m.board.pieces = pieces;

    // loop from H1 to A8
    for (u8 i = 0; i<64; i++) {
        // get bitboard for current piece
        bool is_white;
        u64 piece = get_bin_num(i);
        u64 moves = 0;

        // check team
        is_white = piece & m.board.pieces.at(WHITE);

        if (!is_white && !(piece & m.board.pieces.at(BLACK))) { // no piece
            m.potential_moves.at(i) = 0;
            continue;
        } 

        (is_white 
            ? m.board.white_pieces.push_back(i)
            : m.board.black_pieces.push_back(i));

        switch(map_piece(m.board.pieces, piece)) {
            case PAWNS:
                moves = get_pawn_moves(m.board.pieces, i, is_white);
                m.board.pawns.push_back(i);
                break;
            case ROOKS:
                moves = get_path_moves(m.board.pieces, i, is_white, 
                        opt::STRAIGHT);
                m.board.rooks.push_back(i);
                break;
            case KNIGHTS:
                moves = get_knight_moves(m.board.pieces, i, is_white);
                m.board.knights.push_back(i);
                break;
            case BISHOPS:
                moves = get_path_moves(m.board.pieces, i, is_white, 
                        opt::DIAG);
                m.board.bishops.push_back(i);
                break;
            case QUEENS:
                moves = get_path_moves(m.board.pieces, i, 
                                       is_white, opt::STRAIGHT) 
                        | get_path_moves(m.board.pieces, i, 
                                         is_white, opt::DIAG);
                m.board.queens.push_back(i);
                break;
            case KINGS:
                moves = get_king_moves(m.board.pieces, i, is_white);
                m.board.kings.push_back(i);
                break;
            default:
                break;
        }

        m.potential_moves.at(i) = moves;
    }

    if (!filter_legal) return m;

    // after loop, check which moves are legal, i.e. don't endanger your king

    // loop through all moves of all current team's pieces and check    
    // whether that move endangers the king, if it does, filter
     m = filter(m, m.board.white_pieces, true);
     m = filter(m, m.board.black_pieces, false);

    return m;
}

// unit tests
#define EQUALS(x,y) { if (x != y) std::cout << __FUNCTION__ \
    << " failed on line " << __LINE__ << std::endl \
    << x << " does not equal " << y << "\n" ; }

void run_tests() {
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
    EQUALS(__builtin_ctzl(0x8000000000000000), 63);
    EQUALS(__builtin_ctzl(0x0000000000000001), 0);
    EQUALS(__builtin_ctzl(0x0000001000000000), 36);

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

    Moves m = gen_moves(arrays.initial_positions);

    while (true) {
        for (int i=0; i<64; i++) {
            std::cout << "                                         "
                      << "                                         \n";
        }
        std::cout << "\033[0;64H"; // move cursor up 56 lines

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
