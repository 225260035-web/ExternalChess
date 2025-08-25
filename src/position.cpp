#include "position.h"
#include "thread.h"
#include <sstream>

using namespace std;

static ThreadContext global_thread_ctx;
Position::Position() : side(WHITE), st(new StateInfo{0,0,0}), thread(&global_thread_ctx) {
	// Start position minimal: empty board; caller should set
}

Position& Position::set(const std::string& fen, Color s, StateInfo* state) {
	// Minimal: setup a standard initial position if fen == "startpos"
	side = s;
	st = state ? state : st;
	// Clear
	for(int i=0;i<SQUARE_NB;++i) board[i]=NO_PIECE;
	for(int c=0;c<COLOR_NB;++c){ occupancy[c]=0; nonPawnMat[c]=0; for(int pt=0;pt<PIECE_TYPE_NB;++pt){ pieceBB[c][pt]=0; pieceCounts[c][pt]=0; for(int k=0;k<10;++k) pieceSquares[c][pt][k]=SQ_NONE; }}
	occupancyAll=0; memset(pieceTypeBB,0,sizeof(pieceTypeBB));
	// Simple: place kings to avoid eval asserts
	board[SQ_E1]=W_KING; board[SQ_E8]=B_KING;
	occupancy[WHITE]|=1ULL<<SQ_E1; occupancy[BLACK]|=1ULL<<SQ_E8; occupancyAll=occupancy[WHITE]|occupancy[BLACK];
	pieceBB[WHITE][KING]|=1ULL<<SQ_E1; pieceBB[BLACK][KING]|=1ULL<<SQ_E8;
	pieceTypeBB[KING]|=1ULL<<SQ_E1; pieceTypeBB[KING]|=1ULL<<SQ_E8;
	pieceSquares[WHITE][KING][0]=SQ_E1; pieceSquares[BLACK][KING][0]=SQ_E8; pieceCounts[WHITE][KING]=1; pieceCounts[BLACK][KING]=1;
	st->pawnKey = 0x9e3779b97f4a7c15ULL; // arbitrary
	st->materialKey = 0xC0FFEEULL;
	return *this;
}

template<PieceType PT>
Square Position::square(Color c) const {
	return pieceCounts[c][PT] ? pieceSquares[c][PT][0] : SQ_NONE;
}

template<PieceType PT>
const Square* Position::squares(Color c) const {
	return pieceSquares[c][PT];
}

bool Position::is_on_semiopen_file(Color c, Square s) const {
	Bitboard f = file_bb(file_of(s));
	Bitboard pawns = pieceBB[c][PAWN];
	return (pawns & f) == 0ULL && ((pieceBB[~c][PAWN] & f) != 0ULL);
}

int Position::pawns_on_same_color_squares(Color c, Square bishopSq) const{
	Bitboard pawns = pieceBB[c][PAWN];
	int cnt = 0;
	while(pawns){ Square psq = lsb(pawns); pawns &= pawns-1; cnt += opposite_colors(psq, bishopSq) ? 0 : 1; }
	return cnt;
}

bool Position::pawn_passed(Color c, Square s) const{
	Bitboard mask = 0ULL;
	int r = int(rank_of(s));
	int f = int(file_of(s));
	if(c==WHITE){ for(int rr=r+1; rr<8; ++rr){ for(int df=-1; df<=1; ++df){ int ff=f+df; if(ff>=0&&ff<8) mask |= 1ULL<<(rr*8+ff);} } }
	else { for(int rr=r-1; rr>=0; --rr){ for(int df=-1; df<=1; ++df){ int ff=f+df; if(ff>=0&&ff<8) mask |= 1ULL<<(rr*8+ff);} } }
	return (pieceBB[~c][PAWN] & mask) == 0ULL;
}

Value Position::non_pawn_material(Color c) const{
	return nonPawnMat[c];
}

Score Position::psq_score() const{
	Score s = SCORE_ZERO;
	for(int sq=0; sq<SQUARE_NB; ++sq){ Piece pc = board[sq]; if(pc==NO_PIECE) continue; s = s + PSQT::psq[pc][sq]; }
	return s;
}

// Explicit template instantiations
template Square Position::square<PAWN>(Color) const;
template Square Position::square<KNIGHT>(Color) const;
template Square Position::square<BISHOP>(Color) const;
template Square Position::square<ROOK>(Color) const;
template Square Position::square<QUEEN>(Color) const;
template Square Position::square<KING>(Color) const;

template const Square* Position::squares<PAWN>(Color) const;
template const Square* Position::squares<KNIGHT>(Color) const;
template const Square* Position::squares<BISHOP>(Color) const;
template const Square* Position::squares<ROOK>(Color) const;
template const Square* Position::squares<QUEEN>(Color) const;
template const Square* Position::squares<KING>(Color) const;

