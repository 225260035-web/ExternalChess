/*
  Modern Chess Engine - Turkish Chess Engine
  Main console UCI interface
*/

#include <iostream>
#include <string>
#include <sstream>

#include "position.h"
#include "search.h"
#include "evaluate.h"
#include "bitboard.h"

using namespace Chess;

void printWelcome() {
    std::cout << "Modern Turkish Chess Engine v1.0" << std::endl;
    std::cout << "Advanced chess engine with Stockfish-based evaluation" << std::endl;
    std::cout << "Type 'help' for commands or 'uci' for UCI mode" << std::endl;
    std::cout << std::endl;
}

void printHelp() {
    std::cout << "Available commands:" << std::endl;
    std::cout << "  uci        - Start UCI mode" << std::endl;
    std::cout << "  position   - Set position from FEN" << std::endl;
    std::cout << "  go         - Start search" << std::endl;
    std::cout << "  eval       - Show position evaluation" << std::endl;
    std::cout << "  perft <d>  - Run perft to depth d" << std::endl;
    std::cout << "  quit       - Exit program" << std::endl;
    std::cout << "  help       - Show this help" << std::endl;
}

void runConsoleMode() {
    Position pos;
    StateInfo si;
    pos.set("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", false, &si);
    
    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream ss(line);
        std::string command;
        ss >> command;
        
        if (command == "quit" || command == "exit") {
            break;
        }
        else if (command == "help") {
            printHelp();
        }
        else if (command == "uci") {
            std::cout << "id name Modern Turkish Chess Engine v1.0" << std::endl;
            std::cout << "id author Turkish Chess Engine Team" << std::endl;
            std::cout << "option name Threads type spin default 1 min 1 max 128" << std::endl;
            std::cout << "option name Hash type spin default 16 min 1 max 2048" << std::endl;
            std::cout << "uciok" << std::endl;
            return; // Exit to UCI mode
        }
        else if (command == "position") {
            std::string type;
            ss >> type;
            
            if (type == "startpos") {
                pos.set("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", false, &si);
                std::cout << "Position set to starting position" << std::endl;
            }
            else if (type == "fen") {
                std::string fen;
                std::getline(ss, fen);
                if (!fen.empty()) {
                    fen = fen.substr(1); // Remove leading space
                    pos.set(fen, false, &si);
                    std::cout << "Position set from FEN: " << fen << std::endl;
                }
            }
        }
        else if (command == "eval") {
            Value eval = Eval::evaluate(pos);
            std::cout << "Position evaluation: " << eval << " (" << float(eval) / 100.0f << " pawns)" << std::endl;
            std::cout << "From " << (pos.side_to_move() == WHITE ? "white" : "black") << " perspective" << std::endl;
        }
        else if (command == "go") {
            std::cout << "Starting search..." << std::endl;
            // Simple depth-limited search would go here
            Value eval = Eval::evaluate(pos);
            std::cout << "Quick evaluation: " << eval << std::endl;
        }
        else if (command == "perft") {
            int depth;
            if (ss >> depth && depth > 0 && depth <= 8) {
                std::cout << "Running perft to depth " << depth << std::endl;
                // Perft implementation would go here
                std::cout << "Perft not implemented yet" << std::endl;
            } else {
                std::cout << "Invalid depth. Use 1-8." << std::endl;
            }
        }
        else if (command == "board" || command == "show") {
            std::cout << pos << std::endl;
        }
        else if (!command.empty()) {
            std::cout << "Unknown command: " << command << std::endl;
            std::cout << "Type 'help' for available commands" << std::endl;
        }
    }
}

int main() {
    // Initialize engine subsystems
    Bitboards::init();
    Position::init();
    Search::init();
    Eval::NNUE::init();
    
    printWelcome();
    
    // Check for UCI mode from command line
    std::string firstLine;
    std::getline(std::cin, firstLine);
    
    if (firstLine == "uci") {
        std::cout << "id name Modern Turkish Chess Engine v1.0" << std::endl;
        std::cout << "id author Turkish Chess Engine Team" << std::endl;
        std::cout << "option name Threads type spin default 1 min 1 max 128" << std::endl;
        std::cout << "option name Hash type spin default 16 min 1 max 2048" << std::endl;
        std::cout << "uciok" << std::endl;
        
        // UCI loop would go here
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line == "quit") break;
            if (line == "isready") {
                std::cout << "readyok" << std::endl;
            }
        }
    } else {
        // Put first line back for console mode
        std::cout << "Console mode. Type 'uci' for UCI mode or 'help' for commands." << std::endl;
        
        // Process the first line in console mode
        std::istringstream ss(firstLine);
        std::string command;
        ss >> command;
        
        if (command == "help") {
            printHelp();
        } else if (!command.empty()) {
            std::cout << "Unknown command: " << command << std::endl;
            printHelp();
        }
        
        runConsoleMode();
    }
    
    return 0;
}