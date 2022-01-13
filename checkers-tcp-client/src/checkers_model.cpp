#include "checkers_model.h"

#include <QDebug>


CheckersModel::CheckersModel(QObject *parent) : QAbstractListModel(parent)
{
    engine.reset();
    Board board = engine.board();
    for(const auto& spot: board) {
        if(spot.piece.type == PieceType::MAN) {
            if(spot.piece.color == Color::BLACK) {
//                qDebug() << "Adding black man at index" << spot.index;
                spots.at(spot.index).piece_type = PieceColorType::BlackMan;
            } else {
//                qDebug() << "Adding white man at index" << spot.index;
                spots.at(spot.index).piece_type = PieceColorType::WhiteMan;
            }
        } else if(spot.piece.type == PieceType::KING) {
            if(spot.piece.color == Color::BLACK) {
//                qDebug() << "Adding black king at index" << spot.index;
                spots.at(spot.index).piece_type = PieceColorType::BlackKing;
            } else {
//                qDebug() << "Adding white king at index" << spot.index;
                spots.at(spot.index).piece_type = PieceColorType::WhiteKing;
            }
        }
    }
//    emit dataChanged(QAbstractListModel::index(0), QAbstractListModel::index(64));
}

QVariant CheckersModel::data(const QModelIndex &index, int role) const
{
  if(role == SquareNumberRole) return index.row();
  if(!index.isValid()) return QVariant();

  // handle white squares
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

bool CheckersModel::setData(const QModelIndex &index, const QVariant &value, int role)
{

    const auto& roleNamesHash = roleNames();
//    qDebug() << "Called setData for index" << index << "with role" << roleNamesHash[role] << "set to value" << value;
    if (is_white_square(index.row())) return true;
    const quint8 spot_index = to_spot_index(index.row());

    switch(role) {
    case SquareStateRole: {
        switch(value.toUInt()) {
        case SpotState::Selected: {
            int valid_moves_count = highlightValidMoves(spot_index);
            if(valid_moves_count != 0) emitDataChanged(role);
//                qDebug() << valid_moves_count << "valid moves for spot" << spot_index;
            return true;
        }
        case SpotState::Default: {
//            resetValidMoves(spot_index);
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

Qt::ItemFlags CheckersModel::flags(const QModelIndex &index) const {
    return Qt::ItemIsEditable | QAbstractListModel::flags(index);
}

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

quint8 CheckersModel::highlightValidMoves(SpotIndex spot_index) {
    const auto moves = engine.valid_moves(spot_index);

    if (moves.empty()) return 0;

    spots.at(spot_index).square_state = SpotState::Selected;
    for (const auto& move: moves) {
        spots.at(move.to).square_state = SpotState::PossibleMove;
//        if(move.type & MoveType::CAPTURE) {
//            SpotIndex captured_index = engine.get_captured_index(move);
//            spots.at(captured_index).square_state = SpotState::Captured;
//        }
    }
    return moves.size();
}

void CheckersModel::resetSpotStates()
{
    for (auto& spot: spots) {
        spot.square_state = SpotState::Default;
    }
    emitDataChanged(SquareStateRole);
}

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

void CheckersModel::emitDataChanged(int role) {
    const auto& begin_index = QAbstractListModel::index(0);
    const auto& end_index = QAbstractListModel::index(63);
//    emit dataChanged(begin_index, end_index, {role, IsOccupiedRole});
    emit dataChanged(begin_index, end_index);
}
