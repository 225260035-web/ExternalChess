/*
  Modern Chess Engine - Turkish Chess Engine
  Pawn structure evaluation
*/

#ifndef PAWNS_H_INCLUDED
#define PAWNS_H_INCLUDED

#include "misc.h"
#include "position.h"
#include "types.h"

namespace Chess {

namespace Pawns {

/// Pawns::Entry contains various information about a pawn structure. A lookup
/// to the pawn hash table (performed by calling the probe function) returns a
/// pointer to an Entry object.

struct Entry {

  Score pawn_score(Color c) const { return scores[c]; }
  Bitboard pawn_attacks(Color c) const { return pawnAttacks[c]; }
  Bitboard passed_pawns(Color c) const { return passedPawns[c]; }
  Bitboard pawn_attacks_span(Color c) const { return pawnAttacksSpan[c]; }
  int passed_count() const { return popcount(passedPawns[WHITE] | passedPawns[BLACK]); }
  int blocked_count() const { return blockedCount; }

  int pawns_on_same_color_squares(Color c, Square s) const {
    return pawnsOnSquares[c][!!(DarkSquares & s)];
  }

  bool semiopen_file(Color c, File f) const {
    return !(semiopenFiles[c] & (1 << f));
  }

  bool semiopen_side(Color c, File f, bool leftSide) const {
    return semiopen_file(c, f) && (leftSide ? (semiopenFiles[c] & ((1 << f) - 1))
                                            : (semiopenFiles[c] & ~((1 << (f + 1)) - 1)));
  }

  template<Color Us>
  Score king_safety(const Position& pos) {
    return  kingSquares[Us] == pos.square<KING>(Us) && castlingRights[Us] == pos.castling_rights(Us)
          ? kingSafety[Us] : (kingSafety[Us] = do_king_safety<Us>(pos));
  }

  template<Color Us>
  Score do_king_safety(const Position& pos);

  template<Color Us>
  Score evaluate_shelter(const Position& pos, Square ksq);

  Key key;
  Score scores[COLOR_NB];
  Bitboard passedPawns[COLOR_NB];
  Bitboard pawnAttacks[COLOR_NB];
  Bitboard pawnAttacksSpan[COLOR_NB];
  Square kingSquares[COLOR_NB];
  Score kingSafety[COLOR_NB];
  int castlingRights[COLOR_NB];
  int semiopenFiles[COLOR_NB];
  int pawnsOnSquares[COLOR_NB][COLOR_NB]; // [color][light/dark squares]
  int asymmetry;
  int openFiles;
  int blockedCount;
};

typedef HashTable<Entry, 131072> Table;

Entry* probe(const Position& pos);

} // namespace Pawns

} // namespace Chess

#endif // #ifndef PAWNS_H_INCLUDED