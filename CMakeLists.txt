cmake_minimum_required(VERSION 3.16)

project(chess VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-fsanitize=address,undefined,leak -O0
-ggdb -pthread -gdwarf-4 -DLinux -fPIC -Wno-deprecated -pipe
-fno-elide-type -fdiagnostics-show-template-tree -Wall -Werror -Wextra
-Wpedantic -Wvla -Wextra-semi -Wnull-dereference -Wswitch-enum
-Wduplicated-cond -Wduplicated-branches -rdynamic -Wsuggest-override
-MMD -MP")

file(GLOB sources src/move_generation.cxx src/move_generation.h)

file(GLOB tui_sources ${sources} src/tui/chess.cxx
    src/tui/print_board.cxx src/tui/print_board.h)

file(GLOB api_sources  ${sources} src/backend/web_backend.cxx)

add_executable(chess ${tui_sources})
add_executable(api ${api_sources})


add_custom_target(
    run
    COMMAND ./chess
    DEPENDS chess
)

