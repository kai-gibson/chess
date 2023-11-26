#ifndef MOVE_GENERATION_H
#define MOVE_GENERATION_H

#include <string>
#include "chess.h"

// utility functions
u64 get_bin_num(i8 arr_indx);
u64 moves_for(Moves m, std::vector<u8> v);
u64 moves_for(Moves m, i8 start, i8 end);

bool king_in_danger(Moves m, bool is_white);
std::string index_to_str(u8 index);
i8 str_to_index(std::string loc);

template<class _container, class _Ty>
bool in_list(_container _C, const _Ty& _Val);

i8 map_piece(Pieces p, u64 piece);

// move generation
Move_Piece move_piece(Moves m, i8 start, i8 end, 
    bool is_white, bool gen=true);

u64 get_path_moves(Pieces p, u64 piece_pos, bool is_white, u8 direction);
u64 get_knight_moves(Pieces p, u8 piece_pos, bool is_white);
u64 get_pawn_moves(Pieces p, u8 piece_pos, bool is_white);
u64 get_king_moves(Pieces p, u8 piece_pos, bool is_white);

Moves filter(Moves m, Piece_List pl, bool is_white);
Moves gen_moves(Pieces pieces, bool filter_legal=true);
#endif
