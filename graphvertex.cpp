#include "graphvertex.h"

GraphVertex::GraphVertex(qreal x, qreal y, QGraphicsItem *parent)
    : QGraphicsEllipseItem(x - 10, y - 10, 20, 20, parent), id(-1)
{
    setBrush(QBrush(Qt::blue)); // Начальный цвет вершины
    setPen(QPen(Qt::black));
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
}

void GraphVertex::setId(int newId)
{
    id = newId;
}

int GraphVertex::getId() const
{
    return id;
}

bool GraphVertex::contains(const QPointF &point) const
{
    return QGraphicsEllipseItem::contains(mapFromScene(point));
}

void GraphVertex::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "Vertex clicked, ID:" << id;
    QGraphicsEllipseItem::mousePressEvent(event);
}
