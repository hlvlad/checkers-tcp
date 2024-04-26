#pragma once

#include <cstdint>
#include <array>
#include <string>
#include <bit>

/**
 * @brief The number of spots on the board.
 */
constexpr size_t SPOTS_NUMBER = 32;

/**
 * @brief Type alias for the spot index.
 */
using SpotIndex = uint8_t;

/**
 * @brief Type alias for the bitboard.
 */
using Bitboard = uint32_t;

/**
 * @brief Enum representing the type of a piece.
 */
enum PieceType { MAN, KING };

/**
 * @brief Enum representing the color of a piece.
 */
enum Color { WHITE, BLACK, BOTH };

/**
 * @brief Enum representing the status of the game.
 */
enum GameStatus { GOING, WIN, DRAW, STOPPED };

/**
 * @brief Enum representing the direction of a move.
 */
enum MoveDirection {
    NORTH_WEST = 7,
    NORTH_EAST = 1,
    SOUTH_EAST = -7,
    SOUTH_WEST = -1,
};

/**
 * @brief Enum representing the type of a move.
 */
enum MoveType : uint8_t {
    NORMAL = 0,
    CAPTURE = 1,
    PROMOTION = 2,
    CAPTURE_PROMOTION = CAPTURE | PROMOTION,
    INVALID = 4
};

/**
 * @brief Struct representing a move.
 */
struct Move {
    SpotIndex from; /**< The starting spot index of the move. */
    SpotIndex to; /**< The destination spot index of the move. */
    MoveType type = MoveType::NORMAL; /**< The type of the move. */

    /**
     * @brief Default constructor for Move.
     */
    Move() = default;

    /**
     * @brief Constructor for Move.
     * @param f The starting spot index of the move.
     * @param t The destination spot index of the move.
     * @param type_ The type of the move.
     */
    Move(SpotIndex f, SpotIndex t, MoveType type_) : from(f), to(t), type(type_) {}
};

/**
 * @brief Struct representing a piece.
 */
struct Piece {
    PieceType type = MAN; /**< The type of the piece. */
    Color color = BOTH; /**< The color of the piece. */
};

/**
 * @brief Struct representing an occupied spot.
 */
struct OccupiedSpot {
    Piece piece; /**< The piece occupying the spot. */
    SpotIndex index; /**< The spot index. */
};

/**
 * @brief Bitboard representing the squares occupied by white pieces.
 */
constexpr Bitboard WHITE_PIECES_SQUARES = 0x041c71c3u;

/**
 * @brief Bitboard representing the squares occupied by black pieces.
 */
constexpr Bitboard BLACK_PIECES_SQUARES = 0xe3820c38u;

/**
 * @brief Bitboard representing the base squares for white pieces.
 */
constexpr Bitboard WHITE_BASE = 0x00041041u;

/**
 * @brief Bitboard representing the base squares for black pieces.
 */
constexpr Bitboard BLACK_BASE = 0x82000820u;

/**
 * @brief Array of opposite base bitboards.
 */
constexpr Bitboard OPPOSITE_BASE[BOTH] { BLACK_BASE, WHITE_BASE };

/**
 * @brief Bitboard representing the attacks in the northeast direction.
 */
constexpr Bitboard NE_ATTACKS_MASK = 0x3cfcf0ccu;

/**
 * @brief Bitboard representing the attacks in the northwest direction.
 */
constexpr Bitboard NW_ATTACKS_MASK = std::rotr(NE_ATTACKS_MASK, 6);

/**
 * @brief Bitboard representing the attacks in the southeast direction.
 */
constexpr Bitboard SE_ATTACKS_MASK = std::rotl(NE_ATTACKS_MASK, 8);

/**
 * @brief Bitboard representing the attacks in the southwest direction.
 */
constexpr Bitboard SW_ATTACKS_MASK = std::rotl(NE_ATTACKS_MASK, 2);

/**
 * @brief Bitboard representing the moves in the northeast direction.
 */
constexpr Bitboard NE_MOVES_MASK = 0x7dfdf5ddu;

/**
 * @brief Bitboard representing the moves in the northwest direction.
 */
constexpr Bitboard NW_MOVES_MASK = 0x79fbf3dbu;

/**
 * @brief Bitboard representing the moves in the southeast direction.
 */
constexpr Bitboard SE_MOVES_MASK = std::rotl(NW_MOVES_MASK, 7);

/**
 * @brief Bitboard representing the moves in the southwest direction.
 */
constexpr Bitboard SW_MOVES_MASK = std::rotl(NE_MOVES_MASK, 1);

/**
 * @brief Bitboard representing an empty board.
 */
constexpr Bitboard BITBOARD_EMPTY = 0u;

/**
 * @brief Bitboard representing a full board.
 */
constexpr Bitboard BITBOARD_FULL = ~BITBOARD_EMPTY;

/**
 * @brief Array mapping spot index to bit index.
 */
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

/**
 * @brief Generates a mapping from spot index to bitboard.
 * @return The mapping from spot index to bitboard.
 */
constexpr auto generate_spot_index_to_bit()
{
    std::array<Bitboard, SPOTS_NUMBER> result{};
    for(int spot_index = 0; spot_index < SPOTS_NUMBER; ++spot_index) {
       result[spot_index] = 1ul << spot_index_to_bit_index[spot_index];
    }
    return result;
}

/**
 * @brief Mapping from spot index to bitboard.
 */
constexpr auto spot_index_to_bit = generate_spot_index_to_bit();

/**
 * @brief Generates a mapping from bit index to spot index.
 * @return The mapping from bit index to spot index.
 */
constexpr std::array<SpotIndex, SPOTS_NUMBER> generate_bit_index_to_spot_index() {
    std::array<SpotIndex, SPOTS_NUMBER> result{};
    for (int spot_index = 0; spot_index < 32; ++spot_index) {
        const size_t bit_index = spot_index_to_bit_index[spot_index];
        result[bit_index] = spot_index;
    }
    return result;
}

/**
 * @brief Mapping from bit index to spot index.
 */
constexpr auto bit_index_to_spot_index = generate_bit_index_to_spot_index();

/**
 * @brief Converts a bitboard to a spot index.
 * @param bit The bitboard to convert.
 * @return The spot index corresponding to the bitboard.
 */
constexpr SpotIndex bit_to_spot_index(Bitboard bit) {
   const size_t bit_index = std::countr_zero(bit);
   return bit_index_to_spot_index[bit_index];
}

/**
 * @brief Checks if a spot index is valid.
 * @param index The spot index to check.
 * @return True if the spot index is valid, false otherwise.
 */
constexpr bool is_valid_index(const SpotIndex index) { return index < SPOTS_NUMBER; }

/**
 * @brief Overloads the bitwise NOT operator for the Color enum.
 * @param c The color to negate.
 * @return The negated color.
 */
constexpr Color operator~(Color c) { return Color(c ^ BLACK); }

/**
 * @brief Shifts a bitboard in a given direction.
 * @param bitboard The bitboard to shift.
 * @param direction The direction to shift the bitboard.
 * @return The shifted bitboard.
 */
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

/**
 * @brief Iterator for iterating over the bits of a bitboard and returning the corresponding spot index.
 * @tparam T The type of the bitboard.
 */
template <typename T>
struct SpotsBitIterator {

    /**
     * @brief Constructs a SpotsBitIterator.
     * @param value_ The value of the bitboard.
     */
    constexpr SpotsBitIterator(T value_) : value(value_) {}

    /**
     * @brief Checks if two SpotsBitIterator objects are not equal.
     * @param other The other SpotsBitIterator object to compare.
     * @return True if the two objects are not equal, false otherwise.
     */
    constexpr bool  operator!=(const SpotsBitIterator &other) { return value != other.value; }

    /**
     * @brief Increments the SpotsBitIterator object.
     */
    constexpr void  operator++() { value &= (value - 1); }

    /**
     * @brief Dereferences the SpotsBitIterator object.
     * @return The spot index corresponding to the current bit.
     */
    constexpr T     operator*() const { return bit_to_spot_index(value); }

    /**
     * @brief Returns the beginning of the SpotsBitIterator.
     * @return The beginning of the SpotsBitIterator.
     */
    constexpr SpotsBitIterator begin() { return value; }

    /**
     * @brief Returns the end of the SpotsBitIterator.
     * @return The end of the SpotsBitIterator.
     */
    constexpr SpotsBitIterator end()   { return 0; }

private:
    T value;
};
