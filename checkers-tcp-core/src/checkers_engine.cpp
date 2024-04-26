/**
 * @file checkers_engine.cpp
 * @brief Implementation of the Checkers Engine class.
 */

#include "checkers_engine.h"

/**
 * @brief Default constructor for the Checkers Engine class.
 */
checkers_engine::checkers_engine()
{

}

/**
 * @brief Reset the game state to the initial state.
 */
void checkers_engine::reset()
{
    pieces[WHITE] = WHITE_PIECES_SQUARES;
    pieces[BLACK] = BLACK_PIECES_SQUARES;
    kings = 0;
    turn = WHITE;
}

/**
 * @brief Make a move on the game board.
 * @param move The move to be made.
 */
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

/**
 * @brief Get the list of valid moves for the current game state.
 * @return The list of valid moves.
 */
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

/**
 * @brief Get the list of valid moves for a specific piece.
 * @param from_index The index of the piece.
 * @return The list of valid moves for the piece.
 */
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

   if (captures()) return list;

   if(king_bit) {
	 for (const SpotIndex to : SpotsBitIterator(king_moves(king_bit)))
	   list.emplace_back(from_index, to, MoveType::NORMAL);
   } else {
	 for (const SpotIndex to : SpotsBitIterator(man_moves(piece_bit)))
	   list.emplace_back(from_index, to, spot_index_to_bit[to] & OPPOSITE_BASE[turn] ? MoveType::PROMOTION : MoveType::NORMAL);
   }

   return list;
}

/**
 * @brief Get the current game board state.
 * @return The game board state.
 */
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

/**
 * @brief Get the index of the captured piece in a move.
 * @param move The move.
 * @return The index of the captured piece, or SPOTS_NUMBER if no piece is captured.
 */
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

/**
 * @brief Convert a bitboard representation to a board representation.
 * @param bitboard The bitboard representation.
 * @return The board representation.
 */
Board checkers_engine::bitboard_to_board(Bitboard bitboard)
{
    Board board;

    for (const SpotIndex index : SpotsBitIterator(bitboard)) {
        board.push_back( {{ MAN, WHITE }, index} );
    }

    return board;
}

/**
 * @brief Check if a move is valid.
 * @param move The move to be checked.
 * @return True if the move is valid, false otherwise.
 */
bool checkers_engine::is_valid(const Move& move) const
{
    const auto from_bit = spot_index_to_bit[move.from] & pieces[turn];
    if (!from_bit) return false;
    return (kings & from_bit ?
                (king_moves(from_bit) | king_capture_moves(from_bit)) :
                (man_moves(from_bit) | man_capture_moves(from_bit))) & spot_index_to_bit[move.to];
}

/**
 * @brief Get the bitboard representation of all possible captures.
 * @return The bitboard representation of all possible captures.
 */
Bitboard checkers_engine::captures() const {
    const auto turn_kings = pieces[turn] & kings;
    auto captures = king_capture_moves(turn_kings);

    const auto turn_mans = pieces[turn] & ~kings;
    captures |= man_capture_moves(turn_mans);

    return captures;
}

/**
 * @brief Get the possible moves for a man piece.
 * @param man_bit The bitboard representation of the man piece.
 * @return The bitboard representation of the possible moves.
 */
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

/**
 * @brief Get the possible moves for a king piece.
 * @param king_bit The bitboard representation of the king piece.
 * @return The bitboard representation of the possible moves.
 */
Bitboard checkers_engine::king_moves(Bitboard king_bit) const {
    const auto empty = ~all();
    return (shift(king_bit & NE_MOVES_MASK, NORTH_EAST) & empty) |
           (shift(king_bit & NW_MOVES_MASK, NORTH_WEST) & empty) |
           (shift(king_bit & SE_MOVES_MASK, SOUTH_EAST) & empty) |
           (shift(king_bit & SW_MOVES_MASK, SOUTH_WEST) & empty);
}

/**
 * @brief Get the possible capture moves for a man piece.
 * @param piece_bit The bitboard representation of the man piece.
 * @return The bitboard representation of the possible capture moves.
 */
Bitboard checkers_engine::man_capture_moves(Bitboard piece_bit) const {
    const auto empty = ~all();

    if (turn == WHITE) {
        return (shift(shift(piece_bit & NE_ATTACKS_MASK, NORTH_EAST) & pieces[BLACK], NORTH_EAST) & empty) |
               (shift(shift(piece_bit & NW_ATTACKS_MASK, NORTH_WEST) & pieces[BLACK], NORTH_WEST) & empty);
    } else {
        return (shift(shift(piece_bit & SE_ATTACKS_MASK, SOUTH_EAST) & pieces[WHITE], SOUTH_EAST) & empty) |
               (shift(shift(piece_bit & SW_ATTACKS_MASK, SOUTH_WEST) & pieces[WHITE], SOUTH_WEST) & empty);
    }
}

/**
 * @brief Get the possible capture moves for a king piece.
 * @param piece_bit The bitboard representation of the king piece.
 * @return The bitboard representation of the possible capture moves.
 */
Bitboard checkers_engine::king_capture_moves(Bitboard piece_bit) const {
    const auto empty = ~all();
    const auto opponent = pieces[~turn];

    return (shift(shift(piece_bit & NE_ATTACKS_MASK, NORTH_EAST) & opponent, NORTH_EAST) & empty) |
           (shift(shift(piece_bit & NW_ATTACKS_MASK, NORTH_WEST) & opponent, NORTH_WEST) & empty) |
           (shift(shift(piece_bit & SE_ATTACKS_MASK, SOUTH_EAST) & opponent, SOUTH_EAST) & empty) |
           (shift(shift(piece_bit & SW_ATTACKS_MASK, SOUTH_WEST) & opponent, SOUTH_WEST) & empty);
}
