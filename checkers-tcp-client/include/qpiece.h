#ifndef QPIECE_H
#define QPIECE_H

#include <QQuickItem>
#include "misc.h"

class QPiece : public QQuickItem {
    Q_OBJECT
public:
    QPiece(size_t sq_, PieceType type_, Color color_, QQuickItem *parent = nullptr);

    size_t sq;
    PieceType type;
    Color color;
protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data) override;
};

#endif // QPIECE_H
