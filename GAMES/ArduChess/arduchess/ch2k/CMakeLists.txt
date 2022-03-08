cmake_minimum_required(VERSION 2.8)

project(chess2k)

file(GLOB PGNLIB_FILES "pgnlib/*.cpp" "pgnlib/*.h")
add_library(pgnlib STATIC ${PGNLIB_FILES})
target_include_directories(pgnlib PUBLIC pgnlib)
add_executable(perft perft.cpp ch2k.hpp)
add_executable(compete compete.cpp ch2k.hpp)
add_executable(test_opening_book test_opening_book.cpp ch2k.hpp)
target_compile_definitions(compete PUBLIC
    -DPOSFILE="${CMAKE_CURRENT_SOURCE_DIR}/starting_pos.epd")
add_executable(gen_starting_pos gen_starting_pos.cpp ch2k.hpp)
target_compile_definitions(gen_starting_pos PUBLIC
    -DTEXELFILE="${CMAKE_CURRENT_SOURCE_DIR}/texel.epd"
    -DPOSFILE="${CMAKE_CURRENT_SOURCE_DIR}/starting_pos.epd")
add_executable(texel_tuning texel_tuning.cpp ch2k.hpp)
target_compile_definitions(texel_tuning PUBLIC
    -DTEXELFILE="${CMAKE_CURRENT_SOURCE_DIR}/texel_big.epd")
add_executable(gen_opening_book gen_opening_book.cpp)
target_link_libraries(gen_opening_book pgnlib)
target_compile_definitions(gen_opening_book PUBLIC
    -DPGNFILE="${CMAKE_CURRENT_SOURCE_DIR}/2019-01.pgn")
