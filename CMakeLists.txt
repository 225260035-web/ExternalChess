cmake_minimum_required(VERSION 3.16)
project(ModernTurkishChessEngine VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3 -DNDEBUG -flto -march=native")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -flto -march=native")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /O2 /DNDEBUG /GL")
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Od /DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG /GL")
endif()

# Add definitions for engine features
add_compile_definitions(
    USE_POPCNT=1
    IS_64BIT=1
)

# Find required packages
find_package(Threads REQUIRED)

# Find SFML
find_package(sfml-all 2.5 QUIET)
if(NOT sfml-all_FOUND)
    find_package(SFML 2.5 COMPONENTS graphics window system QUIET)
    if(NOT SFML_FOUND)
        message(WARNING "SFML not found. GUI will not be built. Install SFML 2.5.1 for GUI support.")
        set(BUILD_GUI OFF)
    else()
        set(BUILD_GUI ON)
    endif()
else()
    set(BUILD_GUI ON)
endif()

# Engine core sources
set(ENGINE_SOURCES
    src/types.h
    src/bitboard.h
    src/bitboard.cpp
    src/position.h
    src/position.cpp
    src/movegen.h
    src/evaluate.h
    src/evaluate.cpp
    src/material.h
    src/pawns.h
    src/endgame.h
    src/search.h
    src/search.cpp
    src/misc.h
)

# External evaluation sources (adapted from Stockfish)
set(EVALUATION_SOURCES
    external_eval/evaluate.cpp
    external_eval/material.cpp
    external_eval/pawns.cpp
    external_eval/endgame.cpp
    external_eval/psqt.cpp
    external_eval/bitbase.cpp
)

# Core engine library
add_library(chess_engine_core STATIC ${ENGINE_SOURCES})
target_include_directories(chess_engine_core PUBLIC src)
target_link_libraries(chess_engine_core Threads::Threads)

# Console UCI engine
add_executable(turkish_chess_engine src/main.cpp)
target_link_libraries(turkish_chess_engine chess_engine_core)

# GUI application (if SFML is available)
if(BUILD_GUI)
    set(GUI_SOURCES
        src/ui/chess_ui.h
        src/ui/chess_ui.cpp
        src/ui/main_gui.cpp
    )
    
    add_executable(turkish_chess_gui ${GUI_SOURCES})
    target_link_libraries(turkish_chess_gui chess_engine_core)
    
    if(sfml-all_FOUND)
        target_link_libraries(turkish_chess_gui sfml-graphics sfml-window sfml-system)
    else()
        target_link_libraries(turkish_chess_gui ${SFML_LIBRARIES})
        target_include_directories(turkish_chess_gui PRIVATE ${SFML_INCLUDE_DIR})
    endif()
    
    # Copy assets to build directory
    file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
    
    message(STATUS "GUI application will be built with SFML support")
else()
    message(STATUS "GUI application will NOT be built (SFML not found)")
endif()

# Performance optimizations
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(chess_engine_core PRIVATE
            -ffast-math
            -funroll-loops
            -fomit-frame-pointer
            -DUSE_POPCNT
        )
    endif()
endif()

# Platform-specific optimizations
if(WIN32)
    target_compile_definitions(chess_engine_core PRIVATE _WIN32_WINNT=0x0601)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(chess_engine_core PRIVATE _GNU_SOURCE)
endif()

# Testing (optional)
option(BUILD_TESTS "Build test suite" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation
install(TARGETS turkish_chess_engine
    RUNTIME DESTINATION bin
)

if(BUILD_GUI)
    install(TARGETS turkish_chess_gui
        RUNTIME DESTINATION bin
    )
    install(DIRECTORY assets/
        DESTINATION share/turkish_chess_engine/assets
    )
endif()

# Package configuration
set(CPACK_PACKAGE_NAME "Turkish Chess Engine")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Modern Turkish Chess Engine with Advanced Evaluation")
set(CPACK_PACKAGE_VENDOR "Turkish Chess Engine Team")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

if(WIN32)
    set(CPACK_GENERATOR "NSIS")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
else()
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
endif()

include(CPack)

# Print build information
message(STATUS "")
message(STATUS "Modern Turkish Chess Engine ${PROJECT_VERSION}")
message(STATUS "========================================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ flags: ${CMAKE_CXX_FLAGS}")
if(BUILD_GUI)
    message(STATUS "SFML GUI: Enabled")
else()
    message(STATUS "SFML GUI: Disabled (SFML not found)")
endif()
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")