#pragma once

#include "types.h"
#include "misc.h"
class Position; // ensure Position seen before external headers that reference it
#include "../external_eval/pawns.h"
#include "../external_eval/material.h"

struct ThreadContext {
	HashTable<Material::Entry, 8192> materialTable;
	HashTable<Pawns::Entry, 131072> pawnsTable;
	Score contempt = SCORE_ZERO;
};

class Thread {
public:
	ThreadContext ctx;
};

