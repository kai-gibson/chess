CXX ?= g++
CXXFLAGS = -O0 -fsanitize=address,undefined,leak -ggdb -pthread -DLinux -fPIC -Wno-deprecated -pipe -fno-elide-type -fdiagnostics-show-template-tree -Wall -Werror -Wextra -Wpedantic -Wvla -Wextra-semi -Wnull-dereference -Wswitch-enum -fvar-tracking-assignments -Wduplicated-cond -Wduplicated-branches -rdynamic -Wsuggest-override -MMD -MP

chess: chess.cxx print_board.cxx Makefile
	$(CXX) $(CXXFLAGS) chess.cxx print_board.cxx move_generation.cxx -o chess

run: chess
	./chess

test: chess
	./chess -test
