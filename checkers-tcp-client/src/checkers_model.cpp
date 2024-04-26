/**
 * @file checkers_model.cpp
 * @brief Implementation of the CheckersModel class.
 */

#include "checkers_model.h"

#include <QDebug>

/**
 * @brief Constructs a CheckersModel object.
 * @param parent The parent QObject.
 */
CheckersModel::CheckersModel(QObject *parent) : QAbstractListModel(parent)
{
    engine.reset();
    Board board = engine.board();
    for(const auto& spot: board) {
        if(spot.piece.type == PieceType::MAN) {
            if(spot.piece.color == Color::BLACK) {
                spots.at(spot.index).piece_type = PieceColorType::BlackMan;
            } else {
                spots.at(spot.index).piece_type = PieceColorType::WhiteMan;
            }
        } else if(spot.piece.type == PieceType::KING) {
            if(spot.piece.color == Color::BLACK) {
                spots.at(spot.index).piece_type = PieceColorType::BlackKing;
            } else {
                spots.at(spot.index).piece_type = PieceColorType::WhiteKing;
            }
        }
    }
}
    
/**
 * @brief Returns the number of rows in the model.
 * @param parent The parent index.
 * @return The number of rows in the model.
 */
int CheckersModel::rowCount(const QModelIndex &parent) const {
    return BOARD_SIZE * BOARD_SIZE;
}

/**
 * @brief Returns the data for the given index and role.
 * @param index The index of the item.
 * @param role The role for which to retrieve the data.
 * @return The data for the given index and role.
 */
QVariant CheckersModel::data(const QModelIndex &index, int role) const
{
    if(role == SquareNumberRole) return index.row();
    if(!index.isValid()) return QVariant();

    const bool is_white = is_white_square(index.row());
    if (role == IsSpotRole) return !is_white;
    if (is_white) return QVariant();

    const SpotIndex spot_index = to_spot_index(index.row());
    const SpotData& spot = spots.at(spot_index);

    switch(role) {
        case SpotNumberRole: return spot_index;
        case PieceTypeRole: return spot.piece_type;
        case SquareStateRole: return spot.square_state;
        case IsOccupiedRole: return spot.piece_type != None;
        case HasMovesRole: return !engine.valid_moves(spot_index).empty();
    }

    return QVariant();
}

/**
 * @brief Returns the role names used by the model.
 * @return The role names used by the model.
 */
QHash<int, QByteArray> CheckersModel::roleNames() const
{
    static QHash<int, QByteArray> mapping {
        {IsSpotRole, "is_spot"},
        {SquareNumberRole, "square_number"},
        {SpotNumberRole, "spot_number"},
        {SquareStateRole, "square_state"},
        {PieceTypeRole, "piece_type"},
        {IsOccupiedRole, "is_occupied"},
        {HasMovesRole, "has_moves"}
    };
    return mapping;
}

/**
 * @brief Sets the data for the given index and role.
 * @param index The index of the item.
 * @param value The new value for the item.
 * @param role The role for which to set the data.
 * @return True if the data was set successfully, false otherwise.
 */
bool CheckersModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    const auto& roleNamesHash = roleNames();
    if (is_white_square(index.row())) return true;
    const quint8 spot_index = to_spot_index(index.row());

    switch(role) {
        case SquareStateRole: {
            switch(value.toUInt()) {
                case SpotState::Selected: {
                    int valid_moves_count = highlightValidMoves(spot_index);
                    if(valid_moves_count != 0) emitDataChanged(role);
                    return true;
                }
                case SpotState::Default: {
                    resetSpotStates();
                    emitDataChanged(role);
                    return true;
                }
                default: return true;
            }
        }
        default: return true;
    }
    return false;
}

/**
 * @brief Returns the item flags for the given index.
 * @param index The index of the item.
 * @return The item flags for the given index.
 */
Qt::ItemFlags CheckersModel::flags(const QModelIndex &index) const {
    return Qt::ItemIsEditable | QAbstractListModel::flags(index);
}

/**
 * @brief Makes a move from one spot to another.
 * @param from The index of the spot to move from.
 * @param to The index of the spot to move to.
 * @return The type of the move made.
 */
quint8 CheckersModel::make_move(quint8 from, quint8 to)
{
    for (const auto& valid_move: engine.valid_moves(from)) {
        if(valid_move.from == from && valid_move.to == to) {
            hideValidMoves(from);
            engine.make_move(valid_move);
            const PieceColorType piece_type = PieceColorType(spots[from].piece_type);
            spots[from].piece_type = PieceColorType::None;
            if (valid_move.type & MoveType::PROMOTION) {
                if(spots[from].piece_type == PieceColorType::WhiteMan) {
                    spots[to].piece_type = PieceColorType::WhiteKing;
                } else if(spots[from].piece_type == PieceColorType::BlackMan) {
                    spots[to].piece_type = PieceColorType::BlackKing;
                } else {
                    spots[to].piece_type = piece_type;
                }
            } else {
                spots[to].piece_type = piece_type;
            }
            if(valid_move.type & MoveType::CAPTURE) {
                SpotIndex captured_index = engine.get_captured_index(valid_move);
                qDebug() << "CAPTURED INDEX from:" << valid_move.from << "to:" << valid_move.to << "captured:" << captured_index;
                spots.at(captured_index).piece_type = PieceColorType::None;
            }
            emitDataChanged(PieceTypeRole);
            return valid_move.type;
        }
    }
    return MoveType::INVALID;
}

/**
 * @brief Highlights the valid moves for a spot.
 * @param spot_index The index of the spot.
 * @return The number of valid moves.
 */
quint8 CheckersModel::highlightValidMoves(SpotIndex spot_index) {
    const auto moves = engine.valid_moves(spot_index);

    if (moves.empty()) return 0;

    spots.at(spot_index).square_state = SpotState::Selected;
    for (const auto& move: moves) {
        spots.at(move.to).square_state = SpotState::PossibleMove;
    }
    return moves.size();
}

/**
 * @brief Resets the square states of all spots.
 */
void CheckersModel::resetSpotStates()
{
    for (auto& spot: spots) {
        spot.square_state = SpotState::Default;
    }
    emitDataChanged(SquareStateRole);
}

/**
 * @brief Hides the valid moves for a spot.
 * @param spot_index The index of the spot.
 * @return The number of valid moves.
 */
quint8 CheckersModel::hideValidMoves(SpotIndex spot_index) {
    for(auto& spot: spots) {
        spot.square_state = SpotState::Default;
    }
    const auto moves = engine.valid_moves(spot_index);

    if (moves.empty()) return 0;

    spots.at(spot_index).square_state = SpotState::Default;
    for (const auto& move: moves) {
        spots.at(move.to).square_state = SpotState::Default;
        if(move.type & MoveType::CAPTURE) {
            SpotIndex captured_index = engine.get_captured_index(move);
            spots.at(captured_index).square_state = SpotState::Default;
        }
    }
    return moves.size();
}

/**
 * @brief Emits the dataChanged signal for the given role.
 * @param role The role for which to emit the signal.
 */
void CheckersModel::emitDataChanged(int role) {
    const auto& begin_index = QAbstractListModel::index(0);
    const auto& end_index = QAbstractListModel::index(63);
    emit dataChanged(begin_index, end_index);
}

/**
 * @brief Check if square is white.
 * @param index The index of the square.
 * @return True if the square is white, false otherwise.
 */
bool CheckersModel::is_white_square(quint8 index) {
    const bool is_row_odd = (index / BOARD_SIZE) & 1;
    const bool is_column_odd = index & 1;
    return is_row_odd == is_column_odd;
}

/**
 * @brief Converts a square index to a spot index.
 * @param square_index The index of the square.
 * @return The index of the spot.
*/
quint8 CheckersModel::to_spot_index(quint8 square_index) {
    return square_index / 2;
}
