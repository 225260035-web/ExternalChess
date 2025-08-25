#include "bitboard.h"
#include <cstring>
#include <cmath>

Bitboard PseudoAttacks[PIECE_TYPE_NB][SQUARE_NB];
Bitboard KnightAttacks[SQUARE_NB];
Bitboard KingAttacks[SQUARE_NB];
Bitboard KingFlank[FILE_NB];
Bitboard Center;
Bitboard CenterFiles;
Bitboard QueenSide;
Bitboard KingSide;
Bitboard AllSquares = 0xFFFFFFFFFFFFFFFFULL;
Bitboard Rank1BB = 0x00000000000000FFULL;
Bitboard Rank2BB = 0x000000000000FF00ULL;
Bitboard Rank3BB = 0x0000000000FF0000ULL;
Bitboard Rank4BB = 0x00000000FF000000ULL;
Bitboard Rank5BB = 0x000000FF00000000ULL;
Bitboard Rank6BB = 0x0000FF0000000000ULL;
Bitboard Rank7BB = 0x00FF000000000000ULL;
Bitboard Rank8BB = 0xFF00000000000000ULL;
Bitboard FileABB = 0x0101010101010101ULL;
Bitboard FileBBB = 0x0202020202020202ULL;
Bitboard FileCBB = 0x0404040404040404ULL;
Bitboard FileDBB = 0x0808080808080808ULL;
Bitboard FileEBB = 0x1010101010101010ULL;
Bitboard FileFBB = 0x2020202020202020ULL;
Bitboard FileGBB = 0x4040404040404040ULL;
Bitboard FileHBB = 0x8080808080808080ULL;
Bitboard LineBB[SQUARE_NB][SQUARE_NB];
Bitboard DarkSquares = 0xAA55AA55AA55AA55ULL;

static inline Bitboard knight_attacks_from(int s){
    int r = s / 8, f = s % 8;
    Bitboard a = 0ULL;
    auto add=[&](int rr,int ff){ if(rr>=0&&rr<8&&ff>=0&&ff<8) a |= 1ULL<<(rr*8+ff); };
    add(r+2,f+1); add(r+2,f-1); add(r-2,f+1); add(r-2,f-1);
    add(r+1,f+2); add(r+1,f-2); add(r-1,f+2); add(r-1,f-2);
    return a;
}

static inline Bitboard king_attacks_from(int s){
    int r = s / 8, f = s % 8;
    Bitboard a = 0ULL;
    for(int dr=-1; dr<=1; ++dr) for(int df=-1; df<=1; ++df){ if(dr==0&&df==0) continue; int rr=r+dr, ff=f+df; if(rr>=0&&rr<8&&ff>=0&&ff<8) a |= 1ULL<<(rr*8+ff); }
    return a;
}

void bitboards_init(){
    CenterFiles = FileCBB | FileDBB | FileEBB | FileFBB;
    Center = (FileCBB | FileDBB | FileEBB | FileFBB) & (Rank3BB | Rank4BB | Rank5BB | Rank6BB);
    QueenSide = FileABB | FileBBB | FileCBB | FileDBB;
    KingSide  = FileEBB | FileFBB | FileGBB | FileHBB;

    for(int f=0; f<8; ++f){
        Bitboard mask = (f<=3 ? (FileABB|FileBBB|FileCBB|FileDBB) : (FileEBB|FileFBB|FileGBB|FileHBB));
        KingFlank[f] = mask;
    }

    for(int s=0; s<SQUARE_NB; ++s){
        KnightAttacks[s] = knight_attacks_from(s);
        KingAttacks[s] = king_attacks_from(s);
        PseudoAttacks[KNIGHT][s] = KnightAttacks[s];
        PseudoAttacks[KING][s] = KingAttacks[s];
        PseudoAttacks[BISHOP][s] = 0; // filled on demand via attacks_bb
        PseudoAttacks[ROOK][s] = 0;
    }

    for(int a=0; a<SQUARE_NB; ++a){
        for(int b=0; b<SQUARE_NB; ++b){
            Bitboard line=0ULL;
            int ar=a/8, af=a%8, br=b/8, bf=b%8;
            int dr = (br>ar) - (br<ar);
            int df = (bf>af) - (bf<af);
            if(dr==0 && df==0){ LineBB[a][b]=0ULL; continue; }
            if(!((dr==0) || (df==0) || (std::abs(br-ar)==std::abs(bf-af)))){ LineBB[a][b]=0ULL; continue; }
            int r=ar+dr, f=af+df;
            while(r!=br || f!=bf){ line |= 1ULL << (r*8+f); r+=dr; f+=df; if(r<0||r>7||f<0||f>7) break; }
            LineBB[a][b]=line;
        }
    }
}

