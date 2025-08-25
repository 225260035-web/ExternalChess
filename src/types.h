#pragma once

#include <cstdint>
#include <cstring>
#include <array>

// Minimal Stockfish-like type aliases and enums to satisfy external_eval

enum Color : int { WHITE = 0, BLACK = 1, COLOR_NB = 2 };

inline Color operator~(Color c) { return Color(c ^ 1); }

enum PieceType : int {
	NO_PIECE_TYPE = 0,
	PAWN = 1,
	KNIGHT = 2,
	BISHOP = 3,
	ROOK = 4,
	QUEEN = 5,
	KING = 6,
	ALL_PIECES = 7,
	PIECE_TYPE_NB = 8
};

enum Piece : int {
	NO_PIECE = 0,
	W_PAWN = 1, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
	B_PAWN = 7, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING,
	PIECE_NB = 16
};

inline Piece make_piece(Color c, PieceType pt) { return Piece((c == WHITE ? 0 : 6) + int(pt)); }
inline PieceType type_of(Piece pc) { return pc == NO_PIECE ? NO_PIECE_TYPE : PieceType((int(pc) - 1) % 6 + 1); }
inline Color color_of(Piece pc) { return pc >= B_PAWN ? BLACK : WHITE; }

enum File : int { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NB };
enum Rank : int { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NB };

inline File& operator++(File& f){ f = File(int(f) + 1); return f; }

enum Square : int {
	SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
	SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
	SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
	SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
	SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
	SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
	SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
	SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
	SQUARE_NB, SQ_NONE = SQUARE_NB
};

inline Square operator~(Square s) { return Square(SQ_A1 + (int(SQ_H8) - int(s))); }
inline Square& operator++(Square& s){ s = Square(int(s) + 1); return s; }
inline Piece& operator++(Piece& p){ p = Piece(int(p) + 1); return p; }

using Bitboard = uint64_t;

// Scores and values (mg/eg pairs)
using Value = int;
struct Score { int mg; int eg; };

constexpr Score SCORE_ZERO{0,0};

constexpr Score make_score(int mg, int eg){ return Score{mg, eg}; }
constexpr int mg_value(Score s){ return s.mg; }
constexpr int eg_value(Score s){ return s.eg; }
constexpr Score operator+(Score a, Score b){ return {a.mg + b.mg, a.eg + b.eg}; }
constexpr Score operator-(Score a, Score b){ return {a.mg - b.mg, a.eg - b.eg}; }
constexpr Score operator-(Score a){ return {-a.mg, -a.eg}; }
constexpr Score operator*(Score a, int k){ return {a.mg * k, a.eg * k}; }
constexpr Score operator/(Score a, int k){ return {a.mg / k, a.eg / k}; }
inline Score& operator+=(Score& a, Score b){ a.mg += b.mg; a.eg += b.eg; return a; }
inline Score& operator-=(Score& a, Score b){ a.mg -= b.mg; a.eg -= b.eg; return a; }

constexpr Value VALUE_ZERO = 0;
constexpr Value VALUE_DRAW = 0;
constexpr Value VALUE_MATE_IN_MAX_PLY = 32000;
constexpr Value VALUE_KNOWN_WIN = 20000;

// Piece values (Mg/Eg)
constexpr Value PawnValueMg   = 198;
constexpr Value KnightValueMg = 817;
constexpr Value BishopValueMg = 836;
constexpr Value RookValueMg   = 1270;
constexpr Value QueenValueMg  = 2521;

constexpr Value PawnValueEg   = 213;
constexpr Value KnightValueEg = 846;
constexpr Value BishopValueEg = 857;
constexpr Value RookValueEg   = 1380;
constexpr Value QueenValueEg  = 2682;

// Game phase
enum Phase : int { PHASE_ENDGAME = 0, PHASE_MIDGAME = 128 };
enum { MG = 0, EG = 1, PHASE_NB = 2 };

// Scale factors
using ScaleFactor = int;
constexpr ScaleFactor SCALE_FACTOR_NORMAL = 64;
constexpr ScaleFactor SCALE_FACTOR_NONE = -1;
constexpr ScaleFactor SCALE_FACTOR_DRAW = 0;
constexpr int SCALE_FACTOR_MAX = 64;

// Castling side flags (placeholder)
enum { KING_SIDE = 1, QUEEN_SIDE = 2 };

// Directions
using Direction = int;
constexpr Direction NORTH = 8;
constexpr Direction SOUTH = -8;
constexpr Direction EAST = 1;
constexpr Direction WEST = -1;

// Utility forward decls for masks used by eval
extern Bitboard PseudoAttacks[PIECE_TYPE_NB][SQUARE_NB];
extern Bitboard KingFlank[FILE_NB];
extern Bitboard Center;
extern Bitboard CenterFiles;
extern Bitboard QueenSide;
extern Bitboard KingSide;
extern Bitboard AllSquares;
extern Bitboard Rank1BB, Rank2BB, Rank3BB, Rank4BB, Rank5BB, Rank6BB, Rank7BB, Rank8BB;
extern Bitboard FileABB, FileBBB, FileCBB, FileDBB, FileEBB, FileFBB, FileGBB, FileHBB;

// Hash key type
using Key = uint64_t;

// Forward decls
class Position;
struct StateInfo { int rule50; Key pawnKey; Key materialKey; };

// Helpers used in evaluation files (prototypes)
template<typename T> inline int popcount(T b){ return __builtin_popcountll((uint64_t)b); }
inline bool more_than_one(Bitboard b){ return (b & (b - 1)) != 0; }
template<typename T> inline int distance(T a, T b){ return 0 + (a>b ? a-b : b-a); }
inline File file_of(Square s){ return File(int(s) & 7); }
inline Rank rank_of(Square s){ return Rank(int(s) >> 3); }
inline Bitboard file_bb(File f){ return Bitboard(0x0101010101010101ULL) << int(f); }
inline Bitboard file_bb(Square s){ return file_bb(file_of(s)); }
inline Bitboard rank_bb(Square s){ return Bitboard(0xFFULL) << (8 * int(rank_of(s))); }
inline bool opposite_colors(Square a, Square b){ return ((int(a)^int(b)) & 1) != 0; }

inline Piece operator~(Piece pc){
	if(pc == NO_PIECE) return NO_PIECE;
	return make_piece(Color(color_of(pc) ^ 1), type_of(pc));
}

inline Bitboard operator&(Bitboard b, Square s){ return b & (Bitboard(1) << int(s)); }
inline Bitboard operator|(Bitboard b, Square s){ return b | (Bitboard(1) << int(s)); }
inline Bitboard operator^(Bitboard b, Square s){ return b ^ (Bitboard(1) << int(s)); }
inline Bitboard& operator|=(Bitboard& b, Square s){ b |= (Bitboard(1) << int(s)); return b; }
inline Bitboard& operator&=(Bitboard& b, Square s){ b &= (Bitboard(1) << int(s)); return b; }

inline Square operator+(Square s, Direction d){ return Square(int(s) + int(d)); }
inline Square operator-(Square s, Direction d){ return Square(int(s) - int(d)); }

// Distance helpers for Squares
inline int distance_file(Square a, Square b){ return distance<int>((int)file_of(a), (int)file_of(b)); }
inline int distance_rank(Square a, Square b){ return distance<int>((int)rank_of(a), (int)rank_of(b)); }

template<typename T>
inline T clamp(T v, T lo, T hi){ return v < lo ? lo : (v > hi ? hi : v); }

inline Square make_square(File f, Rank r){ return Square(int(r) * 8 + int(f)); }

inline Square relative_square(Color c, Square s){ return c == WHITE ? s : Square(int(~s)); }
inline File map_to_queenside(File f){ return f < FILE_E ? f : File(FILE_H - (f - FILE_E)); }

// Pawn helpers
template<Color C>
constexpr Direction pawn_push(){ return C == WHITE ? NORTH : SOUTH; }
constexpr Direction pawn_push(Color c){ return c == WHITE ? NORTH : SOUTH; }
inline Rank relative_rank(Color c, Square s){ return c == WHITE ? rank_of(s) : Rank(7 - int(rank_of(s))); }

template<Color C>
inline Bitboard pawn_double_attacks_bb(Bitboard pawns);

template<Color C>
inline Bitboard pawn_attacks_bb(Bitboard pawns);

// Runtime variants used in eval code
inline Bitboard forward_file_bb(Color c, Square s);
inline Bitboard forward_ranks_bb(Color c, Square s);
inline Bitboard pawn_attack_span(Color c, Square s);

inline Bitboard adjacent_files_bb(Square s){
	int f = int(file_of(s));
	Bitboard left = f>0 ? file_bb(File(f-1)) : 0ULL;
	Bitboard right = f<7 ? file_bb(File(f+1)) : 0ULL;
	return left | right;
}

template<int D>
inline Bitboard shift(Bitboard b){
	if constexpr (D >= 0) return b << D;
	else return b >> (-D);
}

inline Bitboard passed_pawn_span(Color c, Square s){
	Bitboard span = 0ULL;
	int r = int(rank_of(s));
	int f = int(file_of(s));
	if(c==WHITE){ for(int rr=r+1; rr<8; ++rr){ for(int df=-1; df<=1; ++df){ int ff=f+df; if(ff>=0&&ff<8) span |= 1ULL<<(rr*8+ff);} } }
	else { for(int rr=r-1; rr>=0; --rr){ for(int df=-1; df<=1; ++df){ int ff=f+df; if(ff>=0&&ff<8) span |= 1ULL<<(rr*8+ff);} } }
	return span;
}

inline Square frontmost_sq(Color c, Bitboard b){ return c==WHITE ? Square(__builtin_ctzll(b)) : Square(63 - __builtin_clzll(b)); }
inline Square lsb(Bitboard b){ return Square(__builtin_ctzll(b)); }
inline Square pop_lsb(Bitboard* b){ Square s = Square(__builtin_ctzll(*b)); *b &= *b - 1; return s; }

// Declarations used by eval, implemented in other headers
extern Bitboard LineBB[SQUARE_NB][SQUARE_NB];
extern Bitboard DarkSquares;

// PSQT
namespace PSQT{ extern Score psq[PIECE_NB][SQUARE_NB]; void init(); }