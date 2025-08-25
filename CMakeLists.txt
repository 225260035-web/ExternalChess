cmake_minimum_required(VERSION 3.16)
project(ModChess LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_UI "Build SFML UI" ON)

add_compile_definitions(NOMINMAX)

include(GNUInstallDirs)

add_library(engine_core
  external_eval/evaluate.cpp
  external_eval/material.cpp
  external_eval/pawns.cpp
  external_eval/psqt.cpp
  external_eval/endgame.cpp
  src/bitboard.cpp
  src/thread.cpp
  src/position.cpp
  src/movegen.cpp
  src/search.cpp
)

target_include_directories(engine_core PUBLIC
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/external_eval
)

target_compile_options(engine_core PRIVATE
  $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic>
)

add_executable(chess_cli src/cli_main.cpp)
target_link_libraries(chess_cli PRIVATE engine_core)

if(BUILD_UI)
  find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
  add_executable(chess_ui ui/main_ui.cpp)
  target_include_directories(chess_ui PRIVATE ${PROJECT_SOURCE_DIR}/src)
  target_link_libraries(chess_ui PRIVATE engine_core sfml-graphics sfml-window sfml-system)
endif()

install(TARGETS chess_cli RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
if(BUILD_UI)
  install(TARGETS chess_ui RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

