#include <iostream>
#include "position.h"
#include "bitboard.h"
#include "search.h"
#include "../external_eval/evaluate.h"

int main(){
    bitboards_init();
    PSQT::init();

    StateInfo st{};
    Position pos;
    pos.set("startpos", WHITE, &st);

    Searcher s;
    auto res = s.search(pos, 4);
    std::cout << "Eval: " << res.eval << ", depth: " << res.depth << "\n";
    return 0;
}

