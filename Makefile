CXX ?= g++
CXXFLAGS = -ggdb -pthread -DLinux -fPIC -Wno-deprecated -pipe -fno-elide-type -fdiagnostics-show-template-tree -Wall -Werror -Wextra -Wpedantic -Wvla -Wextra-semi -Wnull-dereference -Wswitch-enum -fvar-tracking-assignments -Wduplicated-cond -Wduplicated-branches -rdynamic -Wsuggest-override -MMD -MP

chess: chess.cxx print_board.cxx
	$(CXX) $(CXXFLAGS) chess.cxx print_board.cxx -o chess

run: chess
	./chess

test: chess
	./chess -test
