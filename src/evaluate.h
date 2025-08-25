/*
  Modern Chess Engine - Turkish Chess Engine
  Position evaluation functions integrated from Stockfish
*/

#ifndef EVALUATE_H_INCLUDED
#define EVALUATE_H_INCLUDED

#include <string>
#include "types.h"

namespace Chess {

class Position;

namespace Eval {

constexpr Value Tempo = Value(28); // Must be visible to search

std::string trace(const Position& pos);
Value evaluate(const Position& pos);

namespace NNUE {
  void init();
  void verify();
} // namespace NNUE

} // namespace Eval

} // namespace Chess

#endif // #ifndef EVALUATE_H_INCLUDED