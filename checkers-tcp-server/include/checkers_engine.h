#ifndef CHECKERS_ENGINE_H
#define CHECKERS_ENGINE_H

#include "board.h"
#include <vector>

using MoveList  = std::vector<Move>;
using Board     = std::vector<OccupiedSpot>;

class checkers_engine
{
public:
    checkers_engine();
    void set_my_color(const Color color) {my_color = color;}
    void reset();
    void make_move(const Move& move);

    bool is_valid(const Move& move) const;
    MoveList valid_moves() const;
    MoveList valid_moves(SpotIndex spot_index) const;
    void detect_move_type(Move& move) const;
    Board board() const;
    SpotIndex get_captured_index(const Move& move) const;

    Color turn = BOTH;

    // Debug
    Board bitboard_to_board(Bitboard bitboard);
private:
    Color       my_color = WHITE;
    friend std::ostream& operator<<(std::ostream &os, const checkers_engine &e);

    Bitboard all() const { return pieces[WHITE] | pieces[BLACK] | kings; }
    Bitboard captures() const;
    Bitboard man_moves(Bitboard piece_bit) const;
    Bitboard king_moves(Bitboard piece_bit) const;
    Bitboard man_capture_moves(Bitboard piece_bit) const;
    Bitboard king_capture_moves(Bitboard piece_bit) const;

    Bitboard pieces[BOTH] = {};
    Bitboard kings = 0;
};

#endif // CHECKERS_ENGINE_H
