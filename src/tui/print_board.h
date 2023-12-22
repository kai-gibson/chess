#ifndef PRINT_BOARD_H
#define PRINT_BOARD_H
#include <array>
#include "../types.h"

void print_piece_map(u64 board);
void pretty_print_board(std::array<u64, 8> pieces);

#endif //PRINT_BOARD_H
