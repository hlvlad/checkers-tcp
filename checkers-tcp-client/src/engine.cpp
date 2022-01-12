#include "engine.h"
#include <cstddef>
#include <ostream>

void Engine::reset()
{
    pieces[WHITE] = 0x00000000'0055AA55;
    pieces[BLACK] = 0xAA55AA00'00000000;
    kings = 0x00000000'00000000;

    turn = WHITE;
}

void Engine::act(Move move)
{
    const auto from_bitboard = index_to_bitboard(move.from);
    const auto to_bitboard = index_to_bitboard(move.to);


    // Add "to" bit
    pieces[turn] |= to_bitboard;
    if (kings & from_bitboard || move.type & PROMOTION)
        kings |= to_bitboard;

    // Remove "from" bit
    pieces[turn] &= ~from_bitboard;
    kings &= ~from_bitboard;


    if (move.type & CAPTURE) {
        switch (int(move.to) - int(move.from)) {
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
        if (man_capture_moves(move.to) || king_capture_moves(move.to))
            return;
    }
    turn = ~turn;
}

bool Engine::is_valid(Move move) const
{
    return (kings & index_to_bitboard(move.from) ? 
           (king_moves(move.from)   | king_capture_moves(move.from)) : 
           (man_moves(move.from)    | man_capture_moves(move.from)) )
           & index_to_bitboard(move.to);
}

Bitboard Engine::captures() const
{
    const auto empty = ~all();
    const auto opponent = pieces[~turn];
    const auto turn_kings = pieces[turn] & kings;

    auto captures = (shift(shift(turn_kings, NORTH_EAST) & opponent, NORTH_EAST) & empty) |
                    (shift(shift(turn_kings, NORTH_WEST) & opponent, NORTH_WEST) & empty) |
                    (shift(shift(turn_kings, SOUTH_EAST) & opponent, SOUTH_EAST) & empty) |
                    (shift(shift(turn_kings, SOUTH_WEST) & opponent, SOUTH_WEST) & empty);

    if (turn == WHITE) {
        captures |= (shift(shift(pieces[WHITE], NORTH_EAST) & opponent, NORTH_EAST) & empty) |
                    (shift(shift(pieces[WHITE], NORTH_WEST) & opponent, NORTH_WEST) & empty);
    } else {
        captures |= (shift(shift(pieces[BLACK], SOUTH_EAST) & opponent, SOUTH_EAST) & empty) |
                    (shift(shift(pieces[BLACK], SOUTH_WEST) & opponent, SOUTH_WEST) & empty);
    }
    return captures;
}

Bitboard Engine::man_capture_moves(size_t index) const
{
    const auto empty = ~all();

    if (turn == WHITE) {
        return (shift(R_ATTACKS[WHITE][index] & pieces[BLACK], NORTH_EAST) & empty) |
               (shift(L_ATTACKS[WHITE][index] & pieces[BLACK], NORTH_WEST) & empty);
    } else {
        return (shift(R_ATTACKS[BLACK][index] & pieces[WHITE], SOUTH_EAST) & empty) |
               (shift(L_ATTACKS[BLACK][index] & pieces[WHITE], SOUTH_WEST) & empty);
    }
}

Bitboard Engine::king_capture_moves(size_t index) const
{
    const auto empty = ~all();
    const auto opponent = pieces[~turn];

    return (shift(R_ATTACKS[WHITE][index] & opponent, NORTH_EAST) & empty) |
           (shift(L_ATTACKS[WHITE][index] & opponent, NORTH_WEST) & empty) |
           (shift(R_ATTACKS[BLACK][index] & opponent, SOUTH_EAST) & empty) |
           (shift(L_ATTACKS[BLACK][index] & opponent, SOUTH_WEST) & empty);
}

MoveList Engine::valid_moves() const
{
    MoveList list;

    if (captures()) {

        for (const auto from : BitIterator(pieces[turn] & kings))
             for (const auto to : BitIterator(king_capture_moves(from)))
                 list.emplace_back(size_t(from), size_t(to), MoveType::CAPTURE);

        for (const auto from : BitIterator(pieces[turn] & ~kings))
            for (const auto to : BitIterator(man_capture_moves(from)))
                list.emplace_back(size_t(from), size_t(to), index_to_bitboard(to) & OPPOSITE_RANK[turn] ? MoveType::CAPTURE_PROMOTION : MoveType::CAPTURE);

        return list;
    }

    for (const auto from : BitIterator(pieces[turn] & kings))
         for (const auto to : BitIterator(king_moves(from)))
             list.emplace_back(size_t(from), size_t(to), MoveType::NORMAL);

    for (const auto from : BitIterator(pieces[turn] & ~kings))
        for (const auto to : BitIterator(man_moves(from)))
            list.emplace_back(size_t(from), size_t(to), index_to_bitboard(to) & OPPOSITE_RANK[turn] ? MoveType::PROMOTION : MoveType::NORMAL);

    return list;
}

MoveList Engine::valid_moves(SquareIndex from_index) const {
   MoveList list;
   const auto from_bb = index_to_bitboard(from_index);
   if (!(from_bb & pieces[turn])) return list;
   if (captures()) {
       for (const auto to: BitIterator(king_capture_moves(from_index))) {
           list.emplace_back(size_t(from_index), size_t(to), MoveType::CAPTURE);
       }

       for (const auto to: BitIterator(man_capture_moves(from_index))) {
           list.emplace_back(size_t(from_index), size_t(to), index_to_bitboard(to) & OPPOSITE_RANK[turn] ? MoveType::CAPTURE_PROMOTION : MoveType::CAPTURE);
       }
       return list;
   }

   for (const auto to : BitIterator(king_moves(from_index)))
       list.emplace_back(size_t(from_index), size_t(to), MoveType::NORMAL);

   for (const auto to : BitIterator(man_moves(from_index)))
       list.emplace_back(size_t(from_index), size_t(to), index_to_bitboard(to) & OPPOSITE_RANK[turn] ? MoveType::PROMOTION : MoveType::NORMAL);

   return list;
}

Board Engine::board() const
{
    Board board;

    for (const auto index : BitIterator(pieces[WHITE] & ~kings))
        board.push_back( {{ MAN, WHITE }, index} );

    for (const auto index : BitIterator(pieces[WHITE] & kings))
        board.push_back( {{ KING, WHITE }, index} );

    for (const auto index : BitIterator(pieces[BLACK] & ~kings))
        board.push_back( {{ MAN, BLACK }, index} );

    for (const auto index : BitIterator(pieces[BLACK] & kings))
        board.push_back( {{ KING, BLACK }, index} );

    return board;
}

size_t Engine::get_captured_index(const Move &move) const
{

    if (move.type & CAPTURE) {
        const auto from_bitboard = index_to_bitboard(move.from);
        const auto to_bitboard = index_to_bitboard(move.to);

        switch (int(move.to) - int(move.from)) {
            case NORTH_EAST * 2: return lsb(from_bitboard << NORTH_EAST);
            case NORTH_WEST * 2: return lsb(from_bitboard << NORTH_WEST);
            case SOUTH_EAST * 2: return lsb(to_bitboard << NORTH_WEST);
            case SOUTH_WEST * 2: return lsb(to_bitboard << NORTH_EAST);
            default: return 32;
        }
    }
    return 32;
}


std::ostream& operator<<(std::ostream &os, const Engine &engine)
{
    constexpr auto SIDE_STR = "wb-";
    constexpr auto RANK_STR = "12345678";
    constexpr auto FILE_STR = "abcdefgh";

    os << "\nGAME BOARD:\n\n";

    for (int row = 7; row >= 0; --row) {
        os << RANK_STR[row] << "   ";
        for (int field = 0; field <= 7; ++field) {

            size_t index = square(row, field);
            char c = '.';

            if (get(engine.pieces[WHITE], index))
                c = 'w';
            else if (get(engine.pieces[BLACK], index))
                c = 'b';

            if (get(engine.kings, index))
                c = toupper(c);

            os << c << ' ';
        }
        os << '\n';
    }
    os << "\n   ";
    for (int f = 0; f <= 7; ++f)
        os << ' ' << FILE_STR[f];
    os	<< "\n\nside:\t" << SIDE_STR[engine.turn];

    return os;
}
