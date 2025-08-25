/*
  Modern Chess Engine - Turkish Chess Engine
  GUI Main Entry Point
*/

#include "chess_ui.h"
#include "../bitboard.h"
#include "../position.h"
#include "../search.h"
#include "../evaluate.h"

#include <iostream>
#include <exception>

int main() {
    try {
        // Initialize engine subsystems
        Chess::Bitboards::init();
        Chess::Position::init();
        Chess::Search::init();
        Chess::Eval::NNUE::init();
        
        // Create and run the chess UI
        Chess::ChessUI ui;
        ui.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
    
    return 0;
}