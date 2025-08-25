#pragma once

#include <array>
#include <string>
#include "types.h"
#include "bitboard.h"
struct ThreadContext;

class Position {
public:
	Position();

	// Board setup
	Position& set(const std::string& fen, Color side, StateInfo* st);

	// Accessors used by evaluation
	template<PieceType PT> Square square(Color c) const;
	template<PieceType PT> const Square* squares(Color c) const;
	template<PieceType PT> int count(Color c) const { if constexpr (PT == ALL_PIECES) return popcount(occupancy[c]); else return pieceCounts[c][PT]; }
	// Overload: total count for a piece type across both colors
	template<PieceType PT> int count() const { if constexpr (PT == ALL_PIECES) return popcount(occupancyAll); else return pieceCounts[WHITE][PT] + pieceCounts[BLACK][PT]; }

	Color side_to_move() const { return side; }
	Bitboard pieces() const { return occupancyAll; }
	Bitboard pieces(Color c) const { return occupancy[c]; }
	Bitboard pieces(PieceType pt) const { return pieceTypeBB[pt]; }
	Bitboard pieces(Color c, PieceType pt) const { return pieceBB[c][pt]; }
	Bitboard pieces(Color c, PieceType pt1, PieceType pt2) const { return pieceBB[c][pt1] | pieceBB[c][pt2]; }
	Bitboard pieces(PieceType pt1, PieceType pt2) const { return pieceTypeBB[pt1] | pieceTypeBB[pt2]; }

	bool empty(Square s) const { return board[s] == NO_PIECE; }
	Piece piece_on(Square s) const { return board[s]; }

	template<PieceType PT>
	Bitboard attacks_from(Square s) const {
		if constexpr (PT == KNIGHT) return KnightAttacks[s];
		if constexpr (PT == KING) return KingAttacks[s];
		if constexpr (PT == BISHOP) return attacks_bb<BISHOP>(s, occupancyAll);
		if constexpr (PT == ROOK) return attacks_bb<ROOK>(s, occupancyAll);
		if constexpr (PT == QUEEN) return attacks_bb<BISHOP>(s, occupancyAll) | attacks_bb<ROOK>(s, occupancyAll);
		return 0ULL;
	}

	template<PieceType PT>
	Square square() const; // not used

	Bitboard blockers_for_king(Color c) const { return 0ULL; }
	bool slider_blockers(Bitboard /*sliders*/, Square /*s*/, Bitboard& /*pinners*/) const { return false; }
	bool is_chess960() const { return false; }
	bool opposite_bishops() const { return false; }
	int rule50_count() const { return 0; }
	Bitboard checkers() const { return 0ULL; }

	bool is_on_semiopen_file(Color c, Square s) const;
	int castling_rights(Color) const { return 0; }
	bool can_castle(int) const { return false; }

	int pawns_on_same_color_squares(Color c, Square bishopSq) const;

	bool pawn_passed(Color c, Square s) const;

	Value non_pawn_material(Color c) const;
	Value non_pawn_material() const { return nonPawnMat[WHITE] + nonPawnMat[BLACK]; }

	Score psq_score() const;

	// Pawn tables
	template<Color C>
	Bitboard attacks_from(Square s) const; // specialized for pawns only if used

	// Hash keys
	Key pawn_key() const { return st->pawnKey; }
	Key material_key() const { return st->materialKey; }

	ThreadContext* this_thread() const { return thread; }
	ThreadContext* this_thread() { return thread; }

private:
	Color side;
	Piece board[SQUARE_NB]{};
	Bitboard occupancy[COLOR_NB]{};
	Bitboard occupancyAll{};
	Bitboard pieceBB[COLOR_NB][PIECE_TYPE_NB]{};
	Bitboard pieceTypeBB[PIECE_TYPE_NB]{};
	Square pieceSquares[COLOR_NB][PIECE_TYPE_NB][10]{};
	int pieceCounts[COLOR_NB][PIECE_TYPE_NB]{};
	Value nonPawnMat[COLOR_NB]{};
	StateInfo* st;
	ThreadContext* thread;
};

// Simple MoveList placeholder to satisfy endgame.cpp usage
template<int GEN>
class MoveList {
public:
	explicit MoveList(const Position&) {}
	int size() const { return 1; }
};

enum GenType { LEGAL };