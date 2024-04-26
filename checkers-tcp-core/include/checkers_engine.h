#pragma once

#include "board.h"

#include <vector>

using MoveList  = std::vector<Move>;
using Board     = std::vector<OccupiedSpot>;

/**
 * @brief The checkers_engine class represents the game engine for checkers.
 */
class checkers_engine
{
public:
    /**
     * @brief Constructs a checkers_engine object.
     */
    checkers_engine();

    /**
     * @brief Sets the color of the player.
     * @param color The color of the player.
     */
    void set_my_color(const Color color) {my_color = color;}

    /**
     * @brief Resets the game engine to its initial state.
     */
    void reset();

    /**
     * @brief Makes a move on the checkers board.
     * @param move The move to be made.
     */
    void make_move(const Move& move);

    /**
     * @brief Checks if a move is valid.
     * @param move The move to be checked.
     * @return True if the move is valid, false otherwise.
     */
    bool is_valid(const Move& move) const;

    /**
     * @brief Gets a list of all valid moves.
     * @return A list of all valid moves.
     */
    MoveList valid_moves() const;

    /**
     * @brief Gets a list of valid moves for a specific spot on the board.
     * @param spot_index The index of the spot on the board.
     * @return A list of valid moves for the specified spot.
     */
    MoveList valid_moves(SpotIndex spot_index) const;

    /**
     * @brief Detects the type of a move (normal move or capture move).
     * @param move The move to be detected.
     */
    void detect_move_type(Move& move) const;

    /**
     * @brief Gets the current state of the board.
     * @return The current state of the board.
     */
    Board board() const;

    /**
     * @brief Gets the index of the captured spot in a move.
     * @param move The move.
     * @return The index of the captured spot.
     */
    SpotIndex get_captured_index(const Move& move) const;

    Color turn = BOTH; /**< The current turn in the game. */

    // Debug

    /**
     * @brief Converts a bitboard representation of the board to a regular board representation.
     * @param bitboard The bitboard representation of the board.
     * @return The regular board representation.
     */
    Board bitboard_to_board(Bitboard bitboard);

private:
    Color       my_color = WHITE; /**< The color of the player. */
    friend std::ostream& operator<<(std::ostream &os, const checkers_engine &e);

    /**
     * @brief Gets the bitboard representation of all pieces on the board.
     * @return The bitboard representation of all pieces.
     */
    Bitboard all() const { return pieces[WHITE] | pieces[BLACK] | kings; }

    /**
     * @brief Gets the bitboard representation of all capture moves.
     * @return The bitboard representation of all capture moves.
     */
    Bitboard captures() const;

    /**
     * @brief Gets the bitboard representation of all possible moves for a man piece.
     * @param piece_bit The bitboard representation of the man piece.
     * @return The bitboard representation of all possible moves for the man piece.
     */
    Bitboard man_moves(Bitboard piece_bit) const;

    /**
     * @brief Gets the bitboard representation of all possible moves for a king piece.
     * @param piece_bit The bitboard representation of the king piece.
     * @return The bitboard representation of all possible moves for the king piece.
     */
    Bitboard king_moves(Bitboard piece_bit) const;

    /**
     * @brief Gets the bitboard representation of all possible capture moves for a man piece.
     * @param piece_bit The bitboard representation of the man piece.
     * @return The bitboard representation of all possible capture moves for the man piece.
     */
    Bitboard man_capture_moves(Bitboard piece_bit) const;

    /**
     * @brief Gets the bitboard representation of all possible capture moves for a king piece.
     * @param piece_bit The bitboard representation of the king piece.
     * @return The bitboard representation of all possible capture moves for the king piece.
     */
    Bitboard king_capture_moves(Bitboard piece_bit) const;

    Bitboard pieces[BOTH] = {}; /**< The bitboard representation of the pieces on the board. */
    Bitboard kings = 0; /**< The bitboard representation of the king pieces on the board. */
};
