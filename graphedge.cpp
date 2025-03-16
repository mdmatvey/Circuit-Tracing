#include "graphedge.h"
#include <QPen>

GraphEdge::GraphEdge(GraphVertex *startVertex, GraphVertex *endVertex, QGraphicsItem *parent)
    : QGraphicsLineItem(parent), start(startVertex), end(endVertex)
{
    setLine(QLineF(start->pos(), end->pos()));
    setPen(QPen(Qt::black, 2));
}
