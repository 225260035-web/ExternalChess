#pragma once

#include <vector>
#include <cstdint>
#include "types.h"
#include "position.h"
#include "../external_eval/evaluate.h"

struct SearchResult {
    Value eval;
    int depth;
};

class Searcher {
public:
    SearchResult search(Position& pos, int depth);
private:
    Value negamax(Position& pos, int depth, Value alpha, Value beta);
};

