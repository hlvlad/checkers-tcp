#include "../include/checkers_engine.h"


checkers_engine::checkers_engine()
{

}

void checkers_engine::reset()
{
    pieces[WHITE] = WHITE_PIECES_SQUARES;
    pieces[BLACK] = BLACK_PIECES_SQUARES;
    kings = 0;
    turn = WHITE;
}

void checkers_engine::make_move(const Move &move)
{
    const auto from_bitboard = spot_index_to_bit[move.from];
    const auto to_bitboard = spot_index_to_bit[move.to];


    // Add "to" bit
    pieces[turn] |= to_bitboard;
    if (kings & from_bitboard || move.type & PROMOTION)
        kings |= to_bitboard;

    // Remove "from" bit
    pieces[turn] &= ~from_bitboard;
    kings &= ~from_bitboard;


    if (move.type & CAPTURE) {
        switch (int(spot_index_to_bit_index[move.to]) - int(spot_index_to_bit_index[move.from])) {
            case NORTH_EAST * 2:
                pieces[~turn]   &= ~(from_bitboard << NORTH_EAST);
                kings           &= ~(from_bitboard << NORTH_EAST);
                break;
            case NORTH_WEST * 2:
                pieces[~turn]   &= ~(from_bitboard << NORTH_WEST);
                kings           &= ~(from_bitboard << NORTH_WEST);
                break;
            case SOUTH_EAST * 2:
                pieces[~turn]   &= ~(to_bitboard << NORTH_WEST);
                kings           &= ~(to_bitboard << NORTH_WEST);
                break;
            case SOUTH_WEST * 2:
                pieces[~turn]   &= ~(to_bitboard << NORTH_EAST);
                kings           &= ~(to_bitboard << NORTH_EAST);
                break;
        }
        // If another move available with same piece
        if (man_capture_moves(to_bitboard) || king_capture_moves(from_bitboard))
            return;
    }
    turn = ~turn;
}


MoveList checkers_engine::valid_moves() const
{

    MoveList list;

    if (captures()) {

        for (const auto from : SpotsBitIterator(pieces[turn] & kings))
            for (const auto to : SpotsBitIterator(king_capture_moves(spot_index_to_bit[from])))
                list.emplace_back(from, to, MoveType::CAPTURE);

        for (const auto from : SpotsBitIterator(pieces[turn] & ~kings))
            for (const auto to : SpotsBitIterator(man_capture_moves(spot_index_to_bit[from])))
                list.emplace_back(from, to, spot_index_to_bit[to] & OPPOSITE_BASE[turn] ? MoveType::CAPTURE_PROMOTION : MoveType::CAPTURE);

        return list;
    }

    for (const SpotIndex from : SpotsBitIterator(pieces[turn] & kings))
        for (const SpotIndex to : SpotsBitIterator(king_moves(spot_index_to_bit[from])))
            list.emplace_back(from, to, MoveType::NORMAL);

    for (const SpotIndex from : SpotsBitIterator(pieces[turn] & ~kings))
        for (const SpotIndex to : SpotsBitIterator(man_moves(spot_index_to_bit[from])))
            list.emplace_back(from, to, spot_index_to_bit[to] & OPPOSITE_BASE[turn] ? MoveType::PROMOTION : MoveType::NORMAL);

    return list;
}

MoveList checkers_engine::valid_moves(SpotIndex from_index) const
{
   MoveList list;
   const auto piece_bit = spot_index_to_bit[from_index];
   const auto king_bit = piece_bit & kings;
   if (!(piece_bit & pieces[turn])) return list;
   if(king_bit) {
	 for (const SpotIndex to : SpotsBitIterator(king_capture_moves(piece_bit)))
	   list.emplace_back(from_index, to, MoveType::CAPTURE);
   } else {
	 for (const SpotIndex to: SpotsBitIterator(man_capture_moves(piece_bit)))
	   list.emplace_back(from_index, to, spot_index_to_bit[to] & OPPOSITE_BASE[turn] ? MoveType::CAPTURE_PROMOTION : MoveType::CAPTURE);
   }

   if (captures())  return list;

   if(king_bit) {
	 for (const SpotIndex to : SpotsBitIterator(king_moves(king_bit)))
	   list.emplace_back(from_index, to, MoveType::NORMAL);
   } else {
	 for (const SpotIndex to : SpotsBitIterator(man_moves(piece_bit)))
	   list.emplace_back(from_index, to, spot_index_to_bit[to] & OPPOSITE_BASE[turn] ? MoveType::PROMOTION : MoveType::NORMAL);
   }

   return list;
}

Board checkers_engine::board() const
{
    Board board;

    for (const SpotIndex index : SpotsBitIterator(pieces[WHITE] & ~kings)) {
        board.push_back( {{ MAN, WHITE }, index} );
    }

    for (const SpotIndex index : SpotsBitIterator(pieces[WHITE] & kings)) {
        board.push_back( {{ KING, WHITE }, index} );
    }

    for (const SpotIndex index : SpotsBitIterator(pieces[BLACK] & ~kings)) {
        board.push_back( {{ MAN, BLACK },  index} );
    }

    for (const SpotIndex index : SpotsBitIterator(pieces[BLACK] & kings))
        board.push_back( {{ KING, BLACK }, index} );

    return board;

}

SpotIndex checkers_engine::get_captured_index(const Move &move) const
{
    if (move.type & CAPTURE) {
        const auto from_bit = spot_index_to_bit[move.from];
        const auto to_bit = spot_index_to_bit[move.to];
        if(shift(shift(from_bit, NORTH_WEST), NORTH_WEST) & to_bit) return bit_to_spot_index(shift(from_bit, NORTH_WEST));
        if(shift(shift(from_bit, NORTH_EAST), NORTH_EAST) & to_bit) return bit_to_spot_index(shift(from_bit, NORTH_EAST));
        if(shift(shift(from_bit, SOUTH_WEST), SOUTH_WEST) & to_bit) return bit_to_spot_index(shift(from_bit, SOUTH_WEST));
        if(shift(shift(from_bit, SOUTH_EAST), SOUTH_EAST) & to_bit) return bit_to_spot_index(shift(from_bit, SOUTH_EAST));
    }
    return SPOTS_NUMBER;
}

Board checkers_engine::bitboard_to_board(Bitboard bitboard)
{
    Board board;

    for (const SpotIndex index : SpotsBitIterator(bitboard)) {
        board.push_back( {{ MAN, WHITE }, index} );
    }

    return board;
}

bool checkers_engine::is_valid(const Move& move) const
{
    const auto from_bit = spot_index_to_bit[move.from] & pieces[turn];
    if (!from_bit) return false;
    return (kings & from_bit ?
                (king_moves(from_bit) | king_capture_moves(from_bit)) :
                (man_moves(from_bit) | man_capture_moves(from_bit))) & spot_index_to_bit[move.to];
}

Bitboard checkers_engine::captures() const {
    const auto turn_kings = pieces[turn] & kings;
    auto captures = king_capture_moves(turn_kings);

    const auto turn_mans = pieces[turn] & ~kings;
    captures |= man_capture_moves(turn_mans);

    return captures;
}

// FIXME: implement this
Bitboard checkers_engine::man_moves(Bitboard man_bit) const {
    const auto empty = ~all();
    Bitboard moves = BITBOARD_EMPTY;

    if (turn == WHITE) {
        moves = (shift(man_bit & NE_MOVES_MASK, NORTH_EAST) & empty) |
                (shift(man_bit & NW_MOVES_MASK, NORTH_WEST) & empty);
    } else {
        moves = (shift(man_bit & SE_MOVES_MASK, SOUTH_EAST) & empty) |
                (shift(man_bit & SW_MOVES_MASK, SOUTH_WEST) & empty);
    }
    return moves;
}

// FIXME: implement this
Bitboard checkers_engine::king_moves(Bitboard king_bit) const {
    const auto empty = ~all();
    return (shift(king_bit & NE_MOVES_MASK, NORTH_EAST) & empty) |
           (shift(king_bit & NW_MOVES_MASK, NORTH_WEST) & empty) |
           (shift(king_bit & SE_MOVES_MASK, SOUTH_EAST) & empty) |
           (shift(king_bit & SW_MOVES_MASK, SOUTH_WEST) & empty);
}

Bitboard checkers_engine::man_capture_moves(Bitboard piece_bit) const {
//    const auto turn_piece_bit = piece_bit & pieces[turn];
//    if(!turn_piece_bit) return BITBOARD_EMPTY;
    const auto empty = ~all();

    if (turn == WHITE) {
        return (shift(shift(piece_bit & NE_ATTACKS_MASK, NORTH_EAST) & pieces[BLACK], NORTH_EAST) & empty) |
               (shift(shift(piece_bit & NW_ATTACKS_MASK, NORTH_WEST) & pieces[BLACK], NORTH_WEST) & empty);
    } else {
        return (shift(shift(piece_bit & SE_ATTACKS_MASK, SOUTH_EAST) & pieces[WHITE], SOUTH_EAST) & empty) |
               (shift(shift(piece_bit & SW_ATTACKS_MASK, SOUTH_WEST) & pieces[WHITE], SOUTH_WEST) & empty);
    }
}

Bitboard checkers_engine::king_capture_moves(Bitboard piece_bit) const {
//    const auto turn_king_bit = piece_bit & pieces[turn] & kings;
//    if(!turn_king_bit) return BITBOARD_EMPTY;
    const auto empty = ~all();
    const auto opponent = pieces[~turn];

    return (shift(shift(piece_bit & NE_ATTACKS_MASK, NORTH_EAST) & opponent, NORTH_EAST) & empty) |
           (shift(shift(piece_bit & NW_ATTACKS_MASK, NORTH_WEST) & opponent, NORTH_WEST) & empty) |
           (shift(shift(piece_bit & SE_ATTACKS_MASK, SOUTH_EAST) & opponent, SOUTH_EAST) & empty) |
           (shift(shift(piece_bit & SW_ATTACKS_MASK, SOUTH_WEST) & opponent, SOUTH_WEST) & empty);

}
