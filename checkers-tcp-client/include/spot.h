#ifndef SPOT_H
#define SPOT_H

#include <QQuickItem>
#include "misc.h"

class Spot : public QQuickItem {
    Q_OBJECT
public:
    Spot(size_t sq, QQuickItem *parent = nullptr);
protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data) override;
};

#endif // SPOT_H
