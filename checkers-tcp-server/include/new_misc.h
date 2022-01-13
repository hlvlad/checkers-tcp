#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <array>
#include <string>
#include <bit>

constexpr size_t SPOTS_NUMBER = 32;

using SpotIndex = uint_fast8_t;
using Bitboard = uint32_t;

enum PieceType { MAN, KING };
enum Color { WHITE, BLACK, BOTH };
enum GameStatus { GOING, WIN, DRAW, STOPPED };
enum MoveDirection {
    NORTH_WEST = 7,
    NORTH_EAST = 1,
    SOUTH_EAST = -7,
    SOUTH_WEST = -1,
};
enum MoveType : uint8_t {
    NORMAL = 0,
    CAPTURE = 1,
    PROMOTION = 2,
    CAPTURE_PROMOTION = CAPTURE | PROMOTION
};

struct Move {
    SpotIndex from;
    SpotIndex to;
    MoveType type = MoveType::NORMAL;
    Move() = default;
    Move(SpotIndex f, SpotIndex t, MoveType type_) : from(f), to(t), type(type_) {}
};


struct Piece {
    PieceType type = MAN;
    Color color = BOTH;
};

struct OccupiedSpot {
    Piece piece;
    SpotIndex index;
};


constexpr Bitboard WHITE_PIECES_SQUARES = 0x041c71c3u;
constexpr Bitboard BLACK_PIECES_SQUARES = 0xe3820c38u;

constexpr Bitboard WHITE_BASE = 0x00041041u;
constexpr Bitboard BLACK_BASE = 0x82000820u;

constexpr Bitboard OPPOSITE_BASE[BOTH] { BLACK_BASE, WHITE_BASE };

constexpr Bitboard NE_ATTACKS_MASK = 0x3cfcf0ccu;
constexpr Bitboard NW_ATTACKS_MASK = std::rotr(NE_ATTACKS_MASK, 6);
constexpr Bitboard SE_ATTACKS_MASK = std::rotl(NE_ATTACKS_MASK, 8);
constexpr Bitboard SW_ATTACKS_MASK = std::rotl(NE_ATTACKS_MASK, 2);

constexpr Bitboard NE_MOVES_MASK = 0x7dfdf5ddu;
constexpr Bitboard NW_MOVES_MASK = 0x79fbf3dbu;
constexpr Bitboard SE_MOVES_MASK = std::rotl(NW_MOVES_MASK, 7);
constexpr Bitboard SW_MOVES_MASK = std::rotl(NE_MOVES_MASK, 1);

constexpr Bitboard BITBOARD_EMPTY = 0u;
constexpr Bitboard BITBOARD_FULL = ~BITBOARD_EMPTY;

// See https://www.3dkingdoms.com/checkers/bitboards.htm Appendix section for details
constexpr std::array<size_t, SPOTS_NUMBER> spot_index_to_bit_index = {
    11,  5, 31, 25,
  10,  4, 30, 24,
     3, 29, 23, 17,
   2, 28, 22, 16,
    27, 21, 15,  9,
  26, 20, 14,  8,
    19, 13,  7,  1,
  18, 12,  6,  0
};

constexpr auto generate_spot_index_to_bit()
{
    std::array<Bitboard, SPOTS_NUMBER> result{};
    for(int spot_index = 0; spot_index < SPOTS_NUMBER; ++spot_index) {
//    for(int spot_index = 0; spot_index < 1; ++spot_index) {
       result[spot_index] = 1ul << spot_index_to_bit_index[spot_index];
    }
    return result;
}

constexpr auto spot_index_to_bit = generate_spot_index_to_bit();

constexpr std::array<SpotIndex, SPOTS_NUMBER> generate_bit_index_to_spot_index() {
    std::array<SpotIndex, SPOTS_NUMBER> result{};
    for (int spot_index = 0; spot_index < 32; ++spot_index) {
        const size_t bit_index = spot_index_to_bit_index[spot_index];
        result[bit_index] = spot_index;
    }
    return result;
}

constexpr auto bit_index_to_spot_index = generate_bit_index_to_spot_index();

constexpr SpotIndex bit_to_spot_index(Bitboard bit) {
   const size_t bit_index = std::countr_zero(bit);
   return bit_index_to_spot_index[bit_index];
}


constexpr bool is_valid_index(const SpotIndex index)               { return index < SPOTS_NUMBER; }
constexpr Color operator~(Color c)                  { return Color(c ^ BLACK); }

constexpr Bitboard shift(Bitboard bitboard, MoveDirection direction)
{
    switch (direction) {
        case NORTH_EAST: return std::rotl(bitboard, 1);
        case NORTH_WEST: return std::rotl(bitboard, 7);
        case SOUTH_WEST: return std::rotr(bitboard, 1);
        case SOUTH_EAST: return std::rotr(bitboard, 7);
        default: return 0;
    }
}


// Iterates over bits of bitboard and returns spot index for each bit
template <typename T>
struct SpotsBitIterator {

    constexpr SpotsBitIterator(T value_) : value(value_) {}

    constexpr bool  operator!=(const SpotsBitIterator &other) { return value != other.value; }
    constexpr void  operator++()                         { value &= (value - 1); }
    constexpr T     operator*() const                    { return bit_to_spot_index(value); }

    constexpr SpotsBitIterator begin() { return value; }
    constexpr SpotsBitIterator end()   { return 0; }
private:
    T value;
};

#endif // TYPES_H
