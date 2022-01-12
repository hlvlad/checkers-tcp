#ifndef ENGINE_H
#define ENGINE_H

#include "misc.h"
#include <vector>

using MoveList  = std::vector<Move>;
using Board     = std::vector<std::pair<Piece, size_t>>;

struct Engine {

    void        reset();
    void        act(Move move);

    MoveList    valid_moves() const;
    MoveList    valid_moves(SpotIndex spot_index) const;
    Board       board() const;
    size_t      get_captured_index(const Move& move) const;

    Color       turn = BOTH;
private:
    friend std::ostream& operator<<(std::ostream &os, const Engine &e);

    bool        is_valid(Move move) const;

    Bitboard    all() const                    { return pieces[WHITE] | pieces[BLACK] | kings; }
    Bitboard    captures() const;
    Bitboard    man_moves(size_t index) const     { return ATTACKS[turn][index] & ~all(); }
    Bitboard    king_moves(size_t index) const    { return ATTACKS[BOTH][index] & ~all(); }
    Bitboard    man_capture_moves(size_t index) const;
    Bitboard    king_capture_moves(size_t index) const;

    Bitboard    pieces[BOTH] = {};
    Bitboard    kings = 0;
};

#endif // ENGINE_H
