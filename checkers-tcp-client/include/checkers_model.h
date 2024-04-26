/**
 * @file checkers_model.h
 * @brief Contains the declaration of the CheckersModel class and related enums and structs.
 */

#pragma once

#include "checkers_engine.h"

#include <QAbstractListModel>
#include <QObject>

constexpr uint8_t BOARD_SIZE = 8;
constexpr uint8_t SPOT_COUNT = ((BOARD_SIZE * BOARD_SIZE) / 2);


/**
 * @class CheckersModel
 * @brief Represents the model for the checkers game.
 * 
 * The CheckersModel class inherits from QAbstractListModel and provides the necessary functionality
 * to represent the checkers game board as a list model.
 */
class CheckersModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /**
     * @brief The roles used by the model for data representation.
     */
    enum Roles {
        SquareNumberRole = Qt::UserRole, /**< The role for the square number. */
        SpotNumberRole, /**< The role for the spot number. */
        PieceTypeRole, /**< The role for the piece type. */
        SquareStateRole, /**< The role for the square state. */
        IsOccupiedRole, /**< The role for checking if a spot is occupied. */
        IsSpotRole, /**< The role for checking if a square is a spot. */
        HasMovesRole, /**< The role for checking if a spot has valid moves. */
    };

    /**
     * @brief The colors of the squares on the checkers board.
     */
    enum SquareColor {
        WhiteSquare, /**< The color of a white square. */
        BlackSquare /**< The color of a black square. */
    };
    Q_ENUM(SquareColor)

    /**
     * @brief The possible states of a spot on the checkers board.
     */
    enum SpotState : quint8 {
        Default, /**< The default state of a spot. */
        Selected, /**< The state of a spot when it is selected. */
        Captured, /**< The state of a spot when it is captured. */
        PossibleMove /**< The state of a spot when it is a possible move. */
    };
    Q_ENUM(SpotState)

    /**
     * @brief The possible types and colors of the pieces on the checkers board.
     */
    enum PieceColorType : quint8 {
        None, /**< No piece. */
        WhiteMan, /**< White man piece. */
        BlackMan, /**< Black man piece. */
        WhiteKing, /**< White king piece. */
        BlackKing /**< Black king piece. */
    };
    Q_ENUM(PieceColorType)

    /**
     * @brief The data structure representing a spot on the checkers board.
     */
    struct SpotData {
        quint8 piece_type = PieceColorType::None; /**< The type of the piece on the spot. */
        quint8 square_state = SpotState::Default; /**< The state of the spot. */
    };

    /**
     * @brief Constructs a CheckersModel object.
     * @param parent The parent object.
     */
    explicit CheckersModel(QObject *parent = nullptr);

    /**
     * @brief Returns the number of rows in the model.
     * @param parent The parent index.
     * @return The number of rows in the model.
     */
    int rowCount(const QModelIndex &parent) const override ;

    /**
     * @brief Returns the data for the given index and role.
     * @param index The index of the item.
     * @param role The role for which to retrieve the data.
     * @return The data for the given index and role.
     */
    QVariant data(const QModelIndex &index, int role) const override;

    /**
     * @brief Returns the role names used by the model.
     * @return The role names used by the model.
     */
    QHash<int, QByteArray> roleNames() const override;

    /**
     * @brief Sets the data for the given index and role.
     * @param index The index of the item.
     * @param value The new value for the item.
     * @param role The role for which to set the data.
     * @return True if the data was set successfully, false otherwise.
     */
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    /**
     * @brief Returns the item flags for the given index.
     * @param index The index of the item.
     * @return The item flags for the given index.
     */
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    /**
     * @brief Makes a move on the checkers board.
     * @param from The index of the spot to move from.
     * @param to The index of the spot to move to.
     * @return The type of the moved piece.
     */
    Q_INVOKABLE quint8 make_move(quint8 from, quint8 to);

signals:
    /**
     * @brief Signal emitted when a move is made on the checkers board.
     * @param from The index of the spot to move from.
     * @param to The index of the spot to move to.
     * @param type The type of the moved piece.
     */
    void move(quint8 from, quint8 to, quint8 type);

private:
    std::array<SpotData, SPOT_COUNT> spots; /**< The array of spot data representing the checkers board. */
    static bool is_white_square(quint8 index); /**< Helper function to check if a square is white. */
    static quint8 to_spot_index(quint8 square_index); /**< Helper function to convert a square index to a spot index. */
    checkers_engine engine; /**< The checkers engine for game logic. */

    /**
     * @brief Hides the valid moves for the given spot index.
     * @param spot_index The index of the spot.
     * @return The number of valid moves hidden.
     */
    quint8 hideValidMoves(SpotIndex spot_index);

    /**
     * @brief Highlights the valid moves for the given spot index.
     * @param spot_index The index of the spot.
     * @return The number of valid moves highlighted.
     */
    quint8 highlightValidMoves(SpotIndex spot_index);

    /**
     * @brief Resets the spot states to the default state.
     */
    void resetSpotStates();

    /**
     * @brief Emits the dataChanged signal for the specified role.
     * @param role The role for which to emit the signal.
     */
    void emitDataChanged(int role);
};