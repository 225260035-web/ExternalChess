/*
  Modern Chess Engine - Turkish Chess Engine
  Modern Chess UI with SFML 2.5.1
*/

#ifndef CHESS_UI_H_INCLUDED
#define CHESS_UI_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <memory>
#include <string>
#include <vector>

#include "../position.h"
#include "../search.h"
#include "../types.h"

namespace Chess {

class ChessEngine;

class ChessUI {
public:
    ChessUI();
    ~ChessUI();

    void run();
    void handleEvents();
    void update();
    void render();

private:
    // UI Components
    void initializeUI();
    void loadResources();
    void setupBoard();
    void setupControlPanel();
    void setupInfoPanel();

    // Board rendering
    void drawBoard();
    void drawPieces();
    void drawHighlights();
    void drawMoveArrows();
    void drawLastMove();
    void drawCheck();

    // Input handling
    void handleMouseClick(const sf::Vector2i& mousePos);
    void handleBoardClick(int file, int rank);
    void handleControlClick(const sf::Vector2i& mousePos);
    sf::Vector2i getBoardCoordinates(const sf::Vector2i& mousePos);
    Square getSquareFromCoords(int file, int rank);

    // Game state
    void makeMove(Move move);
    void undoMove();
    void newGame();
    void flipBoard();
    void resetGame();
    
    // Engine interaction
    void startEngineSearch();
    void stopEngineSearch();
    void setEngineDepth(int depth);
    void getEngineMove();

    // UI updates
    void updateInfoPanel();
    void updateEvaluationBar();
    void updateMoveList();
    void updateStatus();
    void showEngineThinking();

    // Utility functions
    sf::Vector2f getSquarePosition(Square square);
    std::string moveToString(Move move);
    std::string positionToString();
    Piece getPieceAt(Square square);
    bool isValidMove(Move move);
    std::vector<Move> getLegalMoves();

    // Constants
    static const int BOARD_SIZE = 640;
    static const int SQUARE_SIZE = 80;
    static const int PANEL_WIDTH = 300;
    static const int WINDOW_WIDTH = BOARD_SIZE + PANEL_WIDTH;
    static const int WINDOW_HEIGHT = BOARD_SIZE + 100;
    
    // Colors
    static const sf::Color LIGHT_SQUARE;
    static const sf::Color DARK_SQUARE;
    static const sf::Color HIGHLIGHT_COLOR;
    static const sf::Color MOVE_HIGHLIGHT_COLOR;
    static const sf::Color CHECK_COLOR;
    static const sf::Color LAST_MOVE_COLOR;

    // SFML objects
    sf::RenderWindow window;
    sf::Font font;
    sf::Texture pieceTextures[PIECE_NB];
    
    // UI elements
    sf::RectangleShape boardSquares[64];
    sf::Sprite pieceSprites[64];
    sf::Text statusText;
    sf::Text evaluationText;
    sf::Text depthText;
    sf::Text moveListText;
    sf::RectangleShape evaluationBar;
    sf::RectangleShape controlButtons[6];
    sf::Text buttonTexts[6];

    // Game state
    Position currentPosition;
    StateInfo stateInfo[MAX_PLY];
    int currentStateIndex;
    std::vector<Move> gameHistory;
    std::vector<std::string> gameHistoryPGN;
    
    // UI state
    Square selectedSquare;
    Square highlightedSquares[64];
    int highlightCount;
    bool boardFlipped;
    bool showCoordinates;
    bool showLegalMoves;
    Move lastMove;
    
    // Engine state
    std::unique_ptr<ChessEngine> engine;
    bool engineThinking;
    int searchDepth;
    Value currentEvaluation;
    std::string engineInfo;
    Move bestMove;
    std::vector<Move> principalVariation;
    
    // Threading
    sf::Thread* engineThread;
    sf::Mutex engineMutex;
    bool stopSearch;
    
    // Performance
    sf::Clock frameClock;
    float frameTime;
    int frameCount;
    float fps;
};

class ChessEngine {
public:
    ChessEngine();
    ~ChessEngine();

    void setPosition(const Position& pos);
    void setDepth(int depth);
    void startSearch();
    void stopSearch();
    
    bool isSearching() const { return searching; }
    Move getBestMove() const { return bestMove; }
    Value getEvaluation() const { return evaluation; }
    int getSearchDepth() const { return searchDepth; }
    std::string getInfo() const { return info; }
    std::vector<Move> getPrincipalVariation() const { return pv; }

private:
    void searchThread();
    void updateInfo();

    Position position;
    StateInfo si;
    
    bool searching;
    bool shouldStop;
    int maxDepth;
    int searchDepth;
    Move bestMove;
    Value evaluation;
    std::string info;
    std::vector<Move> pv;
    
    sf::Thread* thread;
    sf::Mutex mutex;
};

} // namespace Chess

#endif // #ifndef CHESS_UI_H_INCLUDED