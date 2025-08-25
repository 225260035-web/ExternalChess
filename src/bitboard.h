#pragma once

#include "types.h"

// Precomputed attack tables and masks
extern Bitboard KnightAttacks[SQUARE_NB];
extern Bitboard KingAttacks[SQUARE_NB];
extern Bitboard PseudoAttacks[PIECE_TYPE_NB][SQUARE_NB];

// Initialization
void bitboards_init();

template<PieceType PT>
inline Bitboard attacks_bb(Square s, Bitboard occ){
	if constexpr (PT == KNIGHT) return KnightAttacks[s];
	if constexpr (PT == KING) return KingAttacks[s];
	if constexpr (PT == BISHOP) {
		// simple sliding along diagonals (naive, sufficient for eval needs)
		Bitboard attacks = 0ULL;
		int sq = (int)s;
		int r = sq / 8, f = sq % 8;
		for(int dr=1,df=1; r+dr<8 && f+df<8; ++dr,++df){ int ss=(r+dr)*8+(f+df); attacks |= 1ULL<<ss; if(occ & (1ULL<<ss)) break; }
		for(int dr=1,df=-1; r+dr<8 && f+df>=0; ++dr,--df){ int ss=(r+dr)*8+(f+df); attacks |= 1ULL<<ss; if(occ & (1ULL<<ss)) break; }
		for(int dr=-1,df=1; r+dr>=0 && f+df<8; --dr,++df){ int ss=(r+dr)*8+(f+df); attacks |= 1ULL<<ss; if(occ & (1ULL<<ss)) break; }
		for(int dr=-1,df=-1; r+dr>=0 && f+df>=0; --dr,--df){ int ss=(r+dr)*8+(f+df); attacks |= 1ULL<<ss; if(occ & (1ULL<<ss)) break; }
		return attacks;
	}
	if constexpr (PT == ROOK) {
		Bitboard attacks = 0ULL;
		int sq = (int)s;
		int r = sq / 8, f = sq % 8;
		for(int dr=1; r+dr<8; ++dr){ int ss=(r+dr)*8+f; attacks|=1ULL<<ss; if(occ & (1ULL<<ss)) break; }
		for(int dr=-1; r+dr>=0; --dr){ int ss=(r+dr)*8+f; attacks|=1ULL<<ss; if(occ & (1ULL<<ss)) break; }
		for(int df=1; f+df<8; ++df){ int ss=r*8+(f+df); attacks|=1ULL<<ss; if(occ & (1ULL<<ss)) break; }
		for(int df=-1; f+df>=0; --df){ int ss=r*8+(f+df); attacks|=1ULL<<ss; if(occ & (1ULL<<ss)) break; }
		return attacks;
	}
	return 0ULL;
}

template<PieceType PT>
inline Bitboard attacks_from(Square s){
	if constexpr (PT == KNIGHT) return KnightAttacks[s];
	if constexpr (PT == KING) return KingAttacks[s];
	return 0ULL;
}

template<Color C>
inline Bitboard pawn_attacks_bb(Bitboard pawns){
	if constexpr (C == WHITE){
		return ((pawns & ~FileABB) << 7) | ((pawns & ~FileHBB) << 9);
	} else {
		return ((pawns & ~FileHBB) >> 7) | ((pawns & ~FileABB) >> 9);
	}
}

template<Color C>
inline Bitboard pawn_double_attacks_bb(Bitboard pawns){
	// double attacks = squares that can be attacked by two pawns from adjacent files
	Bitboard left = (C==WHITE) ? ((pawns & ~FileABB) << 7) : ((pawns & ~FileHBB) >> 7);
	Bitboard right = (C==WHITE) ? ((pawns & ~FileHBB) << 9) : ((pawns & ~FileABB) >> 9);
	return left & right;
}

template<Color C>
inline Bitboard forward_file_bb(Color, Square s){
	Bitboard f = file_bb(file_of(s));
	if constexpr (C == WHITE){
		Bitboard mask = 0ULL; for(int r=int(rank_of(s))+1; r<8; ++r) mask |= (f & (0xFFULL << (r*8)));
		return mask;
	} else {
		Bitboard mask = 0ULL; for(int r=int(rank_of(s))-1; r>=0; --r) mask |= (f & (0xFFULL << (r*8)));
		return mask;
	}
}

inline Bitboard forward_file_bb(Color c, Square s){ return c==WHITE ? forward_file_bb<WHITE>(c, s) : forward_file_bb<BLACK>(c, s); }

template<Color C>
inline Bitboard forward_ranks_bb(Color, Square s){
	if constexpr (C == WHITE){
		Bitboard mask = 0ULL; for(int r=int(rank_of(s))+1; r<8; ++r) mask |= (0xFFULL << (r*8)); return mask;
	} else {
		Bitboard mask = 0ULL; for(int r=int(rank_of(s))-1; r>=0; --r) mask |= (0xFFULL << (r*8)); return mask;
	}
}

inline Bitboard forward_ranks_bb(Color c, Square s){ return c==WHITE ? forward_ranks_bb<WHITE>(c, s) : forward_ranks_bb<BLACK>(c, s); }

template<Color C>
inline Bitboard pawn_attack_span(Color, Square s){
	Bitboard span = 0ULL; int r = int(rank_of(s)); int f = int(file_of(s));
	if constexpr (C == WHITE){ for(int rr=r+1; rr<8; ++rr){ for(int df=-1; df<=1; ++df){ int ff=f+df; if(ff>=0&&ff<8) span |= 1ULL << (rr*8+ff); } } }
	else { for(int rr=r-1; rr>=0; --rr){ for(int df=-1; df<=1; ++df){ int ff=f+df; if(ff>=0&&ff<8) span |= 1ULL << (rr*8+ff); } } }
	return span;
}

inline Bitboard pawn_attack_span(Color c, Square s){ return c==WHITE ? pawn_attack_span<WHITE>(c, s) : pawn_attack_span<BLACK>(c, s); }

