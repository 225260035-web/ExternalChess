/*
  Modern Chess Engine - Turkish Chess Engine
  Modern Chess UI Implementation with SFML 2.5.1
*/

#include "chess_ui.h"
#include "../evaluate.h"
#include "../movegen.h"
#include "../position.h"
#include "../search.h"

#include <iostream>
#include <sstream>
#include <iomanip>

namespace Chess {

// Color constants
const sf::Color ChessUI::LIGHT_SQUARE = sf::Color(240, 217, 181);
const sf::Color ChessUI::DARK_SQUARE = sf::Color(181, 136, 99);
const sf::Color ChessUI::HIGHLIGHT_COLOR = sf::Color(255, 255, 0, 100);
const sf::Color ChessUI::MOVE_HIGHLIGHT_COLOR = sf::Color(0, 255, 0, 100);
const sf::Color ChessUI::CHECK_COLOR = sf::Color(255, 0, 0, 150);
const sf::Color ChessUI::LAST_MOVE_COLOR = sf::Color(255, 255, 0, 80);

ChessUI::ChessUI() 
    : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Modern Turkish Chess Engine", sf::Style::Titlebar | sf::Style::Close)
    , currentStateIndex(0)
    , selectedSquare(SQ_NONE)
    , highlightCount(0)
    , boardFlipped(false)
    , showCoordinates(true)
    , showLegalMoves(true)
    , lastMove(MOVE_NONE)
    , engineThinking(false)
    , searchDepth(10)
    , currentEvaluation(VALUE_ZERO)
    , engineThread(nullptr)
    , stopSearch(false)
    , frameCount(0)
    , fps(0.0f)
{
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);
    
    initializeUI();
    resetGame();
}

ChessUI::~ChessUI() {
    if (engineThread) {
        stopSearch = true;
        engineThread->wait();
        delete engineThread;
    }
}

void ChessUI::initializeUI() {
    loadResources();
    setupBoard();
    setupControlPanel();
    setupInfoPanel();
    
    engine = std::make_unique<ChessEngine>();
}

void ChessUI::loadResources() {
    // Load font
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        // Fallback to system font or create simple font
        std::cout << "Warning: Could not load font from assets/fonts/arial.ttf" << std::endl;
    }
    
    // Load piece textures
    const std::string pieceFiles[PIECE_NB] = {
        "", "wp.png", "wn.png", "wb.png", "wr.png", "wq.png", "wk.png", "",
        "", "bp.png", "bn.png", "bb.png", "br.png", "bq.png", "bk.png"
    };
    
    for (int i = W_PAWN; i <= B_KING; ++i) {
        if (i == 7 || i == 8) continue; // Skip empty slots
        
        std::string filename = "assets/pieces/" + pieceFiles[i];
        if (!pieceTextures[i].loadFromFile(filename)) {
            std::cout << "Warning: Could not load piece texture: " << filename << std::endl;
            // Create simple colored rectangles as fallback
            sf::Image img;
            img.create(SQUARE_SIZE, SQUARE_SIZE, (i < 8) ? sf::Color::White : sf::Color::Black);
            pieceTextures[i].loadFromImage(img);
        }
    }
}

void ChessUI::setupBoard() {
    for (int square = 0; square < 64; ++square) {
        int rank = square / 8;
        int file = square % 8;
        
        sf::Color squareColor = ((rank + file) % 2 == 0) ? LIGHT_SQUARE : DARK_SQUARE;
        
        boardSquares[square].setSize(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
        boardSquares[square].setFillColor(squareColor);
        boardSquares[square].setPosition(file * SQUARE_SIZE, rank * SQUARE_SIZE);
        
        pieceSprites[square].setTexture(pieceTextures[NO_PIECE]);
        pieceSprites[square].setPosition(file * SQUARE_SIZE, rank * SQUARE_SIZE);
        pieceSprites[square].setScale(
            float(SQUARE_SIZE) / pieceTextures[W_PAWN].getSize().x,
            float(SQUARE_SIZE) / pieceTextures[W_PAWN].getSize().y
        );
    }
}

void ChessUI::setupControlPanel() {
    // Control buttons: New Game, Undo, Flip Board, Engine Move, Stop, Settings
    const std::string buttonLabels[6] = {
        "New Game", "Undo Move", "Flip Board", "Engine Move", "Stop Engine", "Settings"
    };
    
    for (int i = 0; i < 6; ++i) {
        controlButtons[i].setSize(sf::Vector2f(120, 30));
        controlButtons[i].setFillColor(sf::Color(200, 200, 200));
        controlButtons[i].setOutlineThickness(1);
        controlButtons[i].setOutlineColor(sf::Color::Black);
        controlButtons[i].setPosition(BOARD_SIZE + 20, 50 + i * 40);
        
        buttonTexts[i].setFont(font);
        buttonTexts[i].setString(buttonLabels[i]);
        buttonTexts[i].setCharacterSize(14);
        buttonTexts[i].setFillColor(sf::Color::Black);
        buttonTexts[i].setPosition(BOARD_SIZE + 25, 55 + i * 40);
    }
}

void ChessUI::setupInfoPanel() {
    // Status text
    statusText.setFont(font);
    statusText.setCharacterSize(16);
    statusText.setFillColor(sf::Color::Black);
    statusText.setPosition(BOARD_SIZE + 20, 300);
    
    // Evaluation text
    evaluationText.setFont(font);
    evaluationText.setCharacterSize(14);
    evaluationText.setFillColor(sf::Color::Black);
    evaluationText.setPosition(BOARD_SIZE + 20, 330);
    
    // Depth text
    depthText.setFont(font);
    depthText.setCharacterSize(14);
    depthText.setFillColor(sf::Color::Black);
    depthText.setPosition(BOARD_SIZE + 20, 350);
    
    // Move list text
    moveListText.setFont(font);
    moveListText.setCharacterSize(12);
    moveListText.setFillColor(sf::Color::Black);
    moveListText.setPosition(BOARD_SIZE + 20, 380);
    
    // Evaluation bar
    evaluationBar.setSize(sf::Vector2f(200, 20));
    evaluationBar.setPosition(BOARD_SIZE + 20, 370);
    evaluationBar.setFillColor(sf::Color::White);
    evaluationBar.setOutlineThickness(1);
    evaluationBar.setOutlineColor(sf::Color::Black);
}

void ChessUI::run() {
    while (window.isOpen()) {
        handleEvents();
        update();
        render();
        
        frameTime = frameClock.restart().asSeconds();
        frameCount++;
        if (frameCount % 60 == 0) {
            fps = 1.0f / frameTime;
        }
    }
}

void ChessUI::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        switch (event.type) {
        case sf::Event::Closed:
            window.close();
            break;
            
        case sf::Event::MouseButtonPressed:
            if (event.mouseButton.button == sf::Mouse::Left) {
                handleMouseClick(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
            }
            break;
            
        case sf::Event::KeyPressed:
            switch (event.key.code) {
            case sf::Keyboard::Escape:
                selectedSquare = SQ_NONE;
                highlightCount = 0;
                break;
            case sf::Keyboard::F:
                flipBoard();
                break;
            case sf::Keyboard::U:
                undoMove();
                break;
            case sf::Keyboard::N:
                newGame();
                break;
            case sf::Keyboard::Enter:
                if (!engineThinking) {
                    startEngineSearch();
                }
                break;
            default:
                break;
            }
            break;
            
        default:
            break;
        }
    }
}

void ChessUI::handleMouseClick(const sf::Vector2i& mousePos) {
    // Check if click is on the board
    if (mousePos.x < BOARD_SIZE && mousePos.y < BOARD_SIZE) {
        sf::Vector2i boardCoords = getBoardCoordinates(mousePos);
        handleBoardClick(boardCoords.x, boardCoords.y);
    }
    // Check if click is on control panel
    else if (mousePos.x >= BOARD_SIZE) {
        handleControlClick(mousePos);
    }
}

void ChessUI::handleBoardClick(int file, int rank) {
    if (boardFlipped) {
        file = 7 - file;
        rank = 7 - rank;
    }
    
    Square clickedSquare = make_square(File(file), Rank(rank));
    
    if (selectedSquare == SQ_NONE) {
        // No square selected, select this square if it has a piece
        Piece piece = currentPosition.piece_on(clickedSquare);
        if (piece != NO_PIECE && color_of(piece) == currentPosition.side_to_move()) {
            selectedSquare = clickedSquare;
            
            // Highlight legal moves
            if (showLegalMoves) {
                highlightCount = 0;
                auto legalMoves = getLegalMoves();
                for (Move move : legalMoves) {
                    if (from_sq(move) == selectedSquare) {
                        highlightedSquares[highlightCount++] = to_sq(move);
                    }
                }
            }
        }
    } else {
        // Square already selected, try to make a move
        Move move = make_move(selectedSquare, clickedSquare);
        
        // Check for promotion
        if (type_of(currentPosition.piece_on(selectedSquare)) == PAWN &&
            relative_rank(currentPosition.side_to_move(), clickedSquare) == RANK_8) {
            move = make<PROMOTION>(selectedSquare, clickedSquare, QUEEN);
        }
        
        if (isValidMove(move)) {
            makeMove(move);
        }
        
        selectedSquare = SQ_NONE;
        highlightCount = 0;
    }
}

void ChessUI::handleControlClick(const sf::Vector2i& mousePos) {
    for (int i = 0; i < 6; ++i) {
        sf::FloatRect buttonRect = controlButtons[i].getGlobalBounds();
        if (buttonRect.contains(mousePos.x, mousePos.y)) {
            switch (i) {
            case 0: newGame(); break;
            case 1: undoMove(); break;
            case 2: flipBoard(); break;
            case 3: 
                if (!engineThinking) {
                    startEngineSearch();
                }
                break;
            case 4: stopEngineSearch(); break;
            case 5: /* Settings dialog */ break;
            }
            break;
        }
    }
}

sf::Vector2i ChessUI::getBoardCoordinates(const sf::Vector2i& mousePos) {
    return sf::Vector2i(mousePos.x / SQUARE_SIZE, mousePos.y / SQUARE_SIZE);
}

Square ChessUI::getSquareFromCoords(int file, int rank) {
    if (boardFlipped) {
        file = 7 - file;
        rank = 7 - rank;
    }
    return make_square(File(file), Rank(rank));
}

void ChessUI::makeMove(Move move) {
    if (currentStateIndex >= MAX_PLY - 1) return;
    
    currentPosition.do_move(move, stateInfo[++currentStateIndex]);
    gameHistory.push_back(move);
    gameHistoryPGN.push_back(moveToString(move));
    lastMove = move;
    
    engine->setPosition(currentPosition);
    updateInfoPanel();
}

void ChessUI::undoMove() {
    if (currentStateIndex > 0 && !gameHistory.empty()) {
        Move lastMove = gameHistory.back();
        currentPosition.undo_move(lastMove);
        currentStateIndex--;
        gameHistory.pop_back();
        gameHistoryPGN.pop_back();
        
        if (!gameHistory.empty()) {
            this->lastMove = gameHistory.back();
        } else {
            this->lastMove = MOVE_NONE;
        }
        
        engine->setPosition(currentPosition);
        updateInfoPanel();
    }
}

void ChessUI::newGame() {
    resetGame();
}

void ChessUI::flipBoard() {
    boardFlipped = !boardFlipped;
}

void ChessUI::resetGame() {
    currentPosition.set("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", false, &stateInfo[0]);
    currentStateIndex = 0;
    gameHistory.clear();
    gameHistoryPGN.clear();
    selectedSquare = SQ_NONE;
    highlightCount = 0;
    lastMove = MOVE_NONE;
    currentEvaluation = VALUE_ZERO;
    
    engine->setPosition(currentPosition);
    updateInfoPanel();
}

void ChessUI::startEngineSearch() {
    if (!engineThinking) {
        engine->setDepth(searchDepth);
        engineThinking = true;
        
        if (engineThread) {
            delete engineThread;
        }
        
        engineThread = new sf::Thread(&ChessEngine::startSearch, engine.get());
        engineThread->launch();
    }
}

void ChessUI::stopEngineSearch() {
    if (engineThinking) {
        engine->stopSearch();
        if (engineThread) {
            engineThread->wait();
            delete engineThread;
            engineThread = nullptr;
        }
        engineThinking = false;
    }
}

void ChessUI::setEngineDepth(int depth) {
    searchDepth = std::clamp(depth, 1, 20);
}

void ChessUI::getEngineMove() {
    if (engine->isSearching()) return;
    
    Move bestMove = engine->getBestMove();
    if (bestMove != MOVE_NONE && isValidMove(bestMove)) {
        makeMove(bestMove);
    }
    
    engineThinking = false;
    currentEvaluation = engine->getEvaluation();
    updateInfoPanel();
}

void ChessUI::update() {
    // Check if engine has finished thinking
    if (engineThinking && !engine->isSearching()) {
        getEngineMove();
    }
    
    updateInfoPanel();
}

void ChessUI::updateInfoPanel() {
    // Update status
    std::string status = "To move: ";
    status += (currentPosition.side_to_move() == WHITE) ? "White" : "Black";
    
    if (currentPosition.checkers()) {
        status += " (in check)";
    }
    
    statusText.setString(status);
    
    // Update evaluation
    std::stringstream evalStream;
    evalStream << "Evaluation: ";
    if (currentEvaluation >= VALUE_MATE_IN_MAX_PLY) {
        evalStream << "Mate in " << (VALUE_MATE - currentEvaluation + 1) / 2;
    } else if (currentEvaluation <= VALUE_MATED_IN_MAX_PLY) {
        evalStream << "Mated in " << (VALUE_MATE + currentEvaluation) / 2;
    } else {
        evalStream << std::fixed << std::setprecision(2) << float(currentEvaluation) / 100.0f;
    }
    evaluationText.setString(evalStream.str());
    
    // Update depth
    depthText.setString("Search depth: " + std::to_string(searchDepth));
    
    // Update move list
    std::string moveList;
    for (size_t i = 0; i < gameHistoryPGN.size(); ++i) {
        if (i % 2 == 0) {
            moveList += std::to_string(i / 2 + 1) + ". ";
        }
        moveList += gameHistoryPGN[i] + " ";
        if (i % 8 == 7) moveList += "\n";
    }
    moveListText.setString(moveList);
    
    // Update evaluation bar
    float evalBarPosition = 0.5f;
    if (abs(currentEvaluation) < VALUE_MATE_IN_MAX_PLY) {
        evalBarPosition = std::clamp(0.5f + float(currentEvaluation) / 1000.0f, 0.0f, 1.0f);
    }
    
    evaluationBar.setSize(sf::Vector2f(200 * evalBarPosition, 20));
    evaluationBar.setFillColor(evalBarPosition > 0.5f ? sf::Color(100, 255, 100) : sf::Color(255, 100, 100));
}

void ChessUI::render() {
    window.clear(sf::Color(250, 250, 250));
    
    drawBoard();
    drawPieces();
    drawHighlights();
    drawLastMove();
    drawCheck();
    
    // Draw UI elements
    for (int i = 0; i < 6; ++i) {
        window.draw(controlButtons[i]);
        window.draw(buttonTexts[i]);
    }
    
    window.draw(statusText);
    window.draw(evaluationText);
    window.draw(depthText);
    window.draw(moveListText);
    window.draw(evaluationBar);
    
    // Draw coordinates if enabled
    if (showCoordinates) {
        // Draw file letters and rank numbers
        // Implementation would go here
    }
    
    window.display();
}

void ChessUI::drawBoard() {
    for (int i = 0; i < 64; ++i) {
        window.draw(boardSquares[i]);
    }
}

void ChessUI::drawPieces() {
    for (Square sq = SQ_A1; sq <= SQ_H8; ++sq) {
        Piece piece = currentPosition.piece_on(sq);
        if (piece != NO_PIECE) {
            sf::Vector2f pos = getSquarePosition(sq);
            
            sf::Sprite sprite;
            sprite.setTexture(pieceTextures[piece]);
            sprite.setPosition(pos);
            sprite.setScale(
                float(SQUARE_SIZE) / pieceTextures[piece].getSize().x,
                float(SQUARE_SIZE) / pieceTextures[piece].getSize().y
            );
            
            window.draw(sprite);
        }
    }
}

void ChessUI::drawHighlights() {
    // Draw selected square highlight
    if (selectedSquare != SQ_NONE) {
        sf::RectangleShape highlight;
        highlight.setSize(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
        highlight.setFillColor(HIGHLIGHT_COLOR);
        highlight.setPosition(getSquarePosition(selectedSquare));
        window.draw(highlight);
    }
    
    // Draw move highlights
    for (int i = 0; i < highlightCount; ++i) {
        sf::RectangleShape highlight;
        highlight.setSize(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
        highlight.setFillColor(MOVE_HIGHLIGHT_COLOR);
        highlight.setPosition(getSquarePosition(highlightedSquares[i]));
        window.draw(highlight);
    }
}

void ChessUI::drawLastMove() {
    if (lastMove != MOVE_NONE) {
        // Highlight from square
        sf::RectangleShape fromHighlight;
        fromHighlight.setSize(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
        fromHighlight.setFillColor(LAST_MOVE_COLOR);
        fromHighlight.setPosition(getSquarePosition(from_sq(lastMove)));
        window.draw(fromHighlight);
        
        // Highlight to square
        sf::RectangleShape toHighlight;
        toHighlight.setSize(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
        toHighlight.setFillColor(LAST_MOVE_COLOR);
        toHighlight.setPosition(getSquarePosition(to_sq(lastMove)));
        window.draw(toHighlight);
    }
}

void ChessUI::drawCheck() {
    if (currentPosition.checkers()) {
        Square kingSquare = currentPosition.square<KING>(currentPosition.side_to_move());
        
        sf::RectangleShape checkHighlight;
        checkHighlight.setSize(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
        checkHighlight.setFillColor(CHECK_COLOR);
        checkHighlight.setPosition(getSquarePosition(kingSquare));
        window.draw(checkHighlight);
    }
}

sf::Vector2f ChessUI::getSquarePosition(Square square) {
    int file = file_of(square);
    int rank = rank_of(square);
    
    if (boardFlipped) {
        file = 7 - file;
        rank = 7 - rank;
    }
    
    return sf::Vector2f(file * SQUARE_SIZE, (7 - rank) * SQUARE_SIZE);
}

std::string ChessUI::moveToString(Move move) {
    // Simple algebraic notation
    std::string from = "";
    from += char('a' + file_of(from_sq(move)));
    from += char('1' + rank_of(from_sq(move)));
    
    std::string to = "";
    to += char('a' + file_of(to_sq(move)));
    to += char('1' + rank_of(to_sq(move)));
    
    return from + to;
}

Piece ChessUI::getPieceAt(Square square) {
    return currentPosition.piece_on(square);
}

bool ChessUI::isValidMove(Move move) {
    auto legalMoves = getLegalMoves();
    return std::find(legalMoves.begin(), legalMoves.end(), move) != legalMoves.end();
}

std::vector<Move> ChessUI::getLegalMoves() {
    MoveList<LEGAL> moveList(currentPosition);
    std::vector<Move> moves;
    for (const auto& move : moveList) {
        moves.push_back(move);
    }
    return moves;
}

// ChessEngine implementation
ChessEngine::ChessEngine() 
    : searching(false)
    , shouldStop(false)
    , maxDepth(10)
    , searchDepth(0)
    , bestMove(MOVE_NONE)
    , evaluation(VALUE_ZERO)
    , thread(nullptr)
{
    position.set("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", false, &si);
}

ChessEngine::~ChessEngine() {
    if (thread) {
        shouldStop = true;
        thread->wait();
        delete thread;
    }
}

void ChessEngine::setPosition(const Position& pos) {
    sf::Lock lock(mutex);
    position = pos;
}

void ChessEngine::setDepth(int depth) {
    maxDepth = std::clamp(depth, 1, 20);
}

void ChessEngine::startSearch() {
    if (searching) return;
    
    searching = true;
    shouldStop = false;
    
    if (thread) {
        delete thread;
    }
    
    thread = new sf::Thread(&ChessEngine::searchThread, this);
    thread->launch();
}

void ChessEngine::stopSearch() {
    shouldStop = true;
    if (thread) {
        thread->wait();
    }
}

void ChessEngine::searchThread() {
    sf::Lock lock(mutex);
    
    // Simple iterative deepening search
    for (int depth = 1; depth <= maxDepth && !shouldStop; ++depth) {
        searchDepth = depth;
        
        MoveList<LEGAL> moveList(position);
        Move currentBest = MOVE_NONE;
        Value currentEval = -VALUE_INFINITE;
        
        for (const auto& move : moveList) {
            if (shouldStop) break;
            
            StateInfo newSt;
            position.do_move(move, newSt);
            
            Value eval = -Eval::evaluate(position);
            
            position.undo_move(move);
            
            if (eval > currentEval) {
                currentEval = eval;
                currentBest = move;
            }
        }
        
        if (currentBest != MOVE_NONE) {
            bestMove = currentBest;
            evaluation = currentEval;
        }
        
        updateInfo();
    }
    
    searching = false;
}

void ChessEngine::updateInfo() {
    std::stringstream ss;
    ss << "Depth: " << searchDepth << " Eval: " << evaluation;
    info = ss.str();
}

} // namespace Chess