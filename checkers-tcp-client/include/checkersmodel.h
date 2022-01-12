#ifndef CHECKERSMODEL_H
#define CHECKERSMODEL_H

#include "checkers_engine.h"

#include <QAbstractListModel>
#include <QObject>

#define BOARD_SIZE 8
#define SPOT_COUNT (BOARD_SIZE * BOARD_SIZE / 2)



class CheckersModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        SquareNumberRole = Qt::UserRole,
        SpotNumberRole,
        PieceTypeRole,
        SquareStateRole,
        IsOccupiedRole,
        IsSpotRole,
        HasMovesRole,
    };

    enum SquareColor {
        WhiteSquare,
        BlackSquare
    };
    Q_ENUM(SquareColor)

    enum SpotState : quint8 {
        Default,
        Selected,
        Captured,
        PossibleMove
    };
    Q_ENUM(SpotState)

    enum PieceColorType : quint8 {
        None,
        WhiteMan,
        BlackMan,
        WhiteKing,
        BlackKing
    };
    Q_ENUM(PieceColorType)

    struct SpotData {
        quint8 piece_type = PieceColorType::None;
        quint8 square_state = SpotState::Default;
    };

    explicit CheckersModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const { return BOARD_SIZE * BOARD_SIZE;}
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    Q_INVOKABLE bool make_move(quint8 from, quint8 to);

signals:
    void move(quint8 from, quint8 to, quint8 type);

private:
    std::array<SpotData, SPOT_COUNT> spots;
    static bool is_white_square(quint8 index) {
      const bool is_row_odd = (index / BOARD_SIZE) & 1;
      const bool is_column_odd = index & 1;
      return is_row_odd == is_column_odd;
    }
    static quint8 to_spot_index(quint8 square_index) {
        return square_index/2;
    }
    checkers_engine engine;

    quint8 hideValidMoves(SpotIndex spot_index);
    quint8 highlightValidMoves(SpotIndex spot_index);
    void resetSpotStates();
    void emitDataChanged(int role);
};

#endif // CHECKERSMODEL_H
