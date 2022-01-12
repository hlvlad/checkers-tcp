#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <array>
#include <string>
#if __cplusplus > 201703L
#include <bit>
#endif

constexpr size_t SQUARE_SIZE = 60; //px
constexpr size_t SQUARES_NUMBER = 64;

using SquareIndex = uint8_t;
using Bitboard = uint64_t;

enum PieceType   { DEAD, MAN, KING };
enum Color  { WHITE, BLACK, BOTH };
enum GameStatus { GOING, WIN, DRAW, STOPPED };
enum MoveDirection {
    NORTH =  8,
    EAST  =  1,
    SOUTH = -NORTH,
    WEST  = -EAST,

    NORTH_EAST = NORTH + EAST,
    SOUTH_EAST = SOUTH + EAST,
    SOUTH_WEST = SOUTH + WEST,
    NORTH_WEST = NORTH + WEST
};
enum MoveType : uint8_t {
    NORMAL = 0,
    CAPTURE = 1,
    PROMOTION = 2,
    CAPTURE_PROMOTION = CAPTURE | PROMOTION
};

struct Move {
    SquareIndex from;
    SquareIndex to;
    MoveType type = MoveType::NORMAL;
#if __cplusplus > 201703L
    bool operator<=>(const Move &rhs) const = default;
#else
    Move() = default;
    Move(SquareIndex f, SquareIndex t, MoveType type_) : from(f), to(t), type(type_) {}
#endif
};

struct Piece {
    PieceType type = DEAD;
    Color color = BOTH;
};

constexpr Bitboard LIGHT_SQUARES    = 0x55AA55AA55AA55AAULL;
constexpr Bitboard DARK_SQUARES     = 0xAA55AA55AA55AA55ULL;
constexpr Bitboard BORDER_SQUARES   = 0xFF818181818181FFULL;

constexpr Bitboard FILE_A_BB = 0x0101010101010101ULL;
constexpr Bitboard FILE_B_BB = FILE_A_BB << 1;
constexpr Bitboard FILE_C_BB = FILE_A_BB << 2;
constexpr Bitboard FILE_D_BB = FILE_A_BB << 3;
constexpr Bitboard FILE_E_BB = FILE_A_BB << 4;
constexpr Bitboard FILE_F_BB = FILE_A_BB << 5;
constexpr Bitboard FILE_G_BB = FILE_A_BB << 6;
constexpr Bitboard FILE_H_BB = FILE_A_BB << 7;

constexpr Bitboard RANK_1_BB = 0xFFULL;
constexpr Bitboard RANK_2_BB = RANK_1_BB << (8 * 1);
constexpr Bitboard RANK_3_BB = RANK_1_BB << (8 * 2);
constexpr Bitboard RANK_4_BB = RANK_1_BB << (8 * 3);
constexpr Bitboard RANK_5_BB = RANK_1_BB << (8 * 4);
constexpr Bitboard RANK_6_BB = RANK_1_BB << (8 * 5);
constexpr Bitboard RANK_7_BB = RANK_1_BB << (8 * 6);
constexpr Bitboard RANK_8_BB = RANK_1_BB << (8 * 7);

constexpr Bitboard OPPOSITE_RANK[] = { RANK_8_BB, RANK_1_BB };

constexpr auto generate_square_bitboards()
{
    std::array<Bitboard, SQUARES_NUMBER> squares_bitboards = {};
    for (SquareIndex sq = 0; sq < SQUARES_NUMBER; ++sq)
        squares_bitboards[sq] = Bitboard(1ULL << sq);
    return squares_bitboards;
}
constexpr auto SQUARE_BITBOARDS = generate_square_bitboards();

constexpr SquareIndex square(const int row, const int f)   { return row * 8 + f; }
constexpr bool is_valid_index(const SquareIndex index)               { return index < 64; }
constexpr Color operator~(Color c)                  { return Color(c ^ BLACK); }
constexpr Bitboard index_to_bitboard(SquareIndex index)              { return SQUARE_BITBOARDS[index]; }
constexpr void set(Bitboard &bb, SquareIndex index)         { bb |= index_to_bitboard(index); }
constexpr void clr(Bitboard &bb, SquareIndex index)         { bb &= ~index_to_bitboard(index); }
constexpr bool get(Bitboard bb, SquareIndex index)          { return bb & index_to_bitboard(index); }

#if __cplusplus > 201703L
constexpr int count(Bitboard bb)                    { return std::popcount(bb); }
constexpr int lsb(Bitboard bb)                      { return std::countr_zero(bb);  }
#else
constexpr int count(Bitboard bb)                    { int cnt = 0; while (bb) { cnt++; bb &= bb - 1; } return cnt; }
constexpr int lsb(Bitboard bb)                      { return count((bb & -bb) - 1);  }
#endif

constexpr Bitboard shift(Bitboard b, MoveDirection direction)
{
    switch (direction) {
//        case NORTH:      return  b << 8;
//        case SOUTH:      return  b >> 8;
//        case EAST:       return (b & ~FILE_H_BB) << 1;
//        case WEST:       return (b & ~FILE_A_BB) >> 1;
        case NORTH_EAST: return (b & ~FILE_H_BB) << 9;
        case NORTH_WEST: return (b & ~FILE_A_BB) << 7;
        case SOUTH_EAST: return (b & ~FILE_H_BB) >> 7;
        case SOUTH_WEST: return (b & ~FILE_A_BB) >> 9;
        default: return 0;
    }
}

#if __cplusplus > 201703L
constexpr Bitboard shift(Bitboard bitboard, auto directions)
#else
template<class T>
constexpr Bitboard shift(Bitboard bitboard, T directions)
#endif
{
    Bitboard result_bitboard = 0;
    for (const auto direction : directions)
        result_bitboard |= shift(bitboard, direction);
    return result_bitboard;
}

#if __cplusplus > 201703L
constexpr auto generate_attacks(auto direction)
#else
template<class T>
constexpr auto generate_attacks(T direction)
#endif
{
    using namespace std;

    array<array<Bitboard, 64>, 3> attacks = {};

    for (SquareIndex square_index = 0; square_index < SQUARES_NUMBER; ++square_index) {

        const auto bb = index_to_bitboard(square_index);

        attacks[WHITE][square_index] = shift(bb, direction[WHITE]);
        attacks[BLACK][square_index] = shift(bb, direction[BLACK]);
        attacks[BOTH][square_index] = attacks[WHITE][square_index] | attacks[BLACK][square_index];
    }
    return attacks;
}
constexpr auto R_ATTACKS = generate_attacks(std::array{NORTH_EAST, SOUTH_EAST});
constexpr auto L_ATTACKS = generate_attacks(std::array{NORTH_WEST, SOUTH_WEST});
constexpr auto ATTACKS = generate_attacks(std::array{std::array{NORTH_EAST, NORTH_WEST}, std::array{SOUTH_EAST, SOUTH_WEST}});

inline std::string to_str(Bitboard bitboard)
{
    std::string str;
    for (int rank = 7; rank >= 0; --rank) {
        str += '|';
        for (int file = 0; file <= 7; ++file) {
            str += get(bitboard, square(rank, file)) ? 'X' : '-';
            str += '|';
        }
        str += '\n';
    }
    return str;
}

template <typename T>
struct BitIterator {

    constexpr BitIterator(T value_) : value(value_) {}

    constexpr bool  operator!=(const BitIterator &other) { return value != other.value; }
    constexpr void  operator++()                         { value &= (value - 1); }
    constexpr T     operator*() const                    { return lsb(value); }

    constexpr BitIterator begin() { return value; }
    constexpr BitIterator end()   { return 0; }
private:
    T value;
};

#endif // TYPES_H
