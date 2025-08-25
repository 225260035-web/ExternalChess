#include "search.h"

Value Searcher::negamax(Position& pos, int depth, Value alpha, Value beta){
    if(depth == 0){
        return Eval::evaluate(pos);
    }
    // Placeholder: no movegen, just static eval to keep structure
    Value v = Eval::evaluate(pos);
    if(v >= beta) return v;
    if(v > alpha) alpha = v;
    return alpha;
}

SearchResult Searcher::search(Position& pos, int depth){
    Value v = negamax(pos, depth, -VALUE_MATE_IN_MAX_PLY, VALUE_MATE_IN_MAX_PLY);
    return { v, depth };
}

