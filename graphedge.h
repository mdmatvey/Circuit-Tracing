#ifndef GRAPHEDGE_H
#define GRAPHEDGE_H

#include <QGraphicsLineItem>
#include "graphvertex.h"

class GraphEdge : public QGraphicsLineItem
{
public:
    GraphEdge(GraphVertex *startVertex, GraphVertex *endVertex, QGraphicsItem *parent = nullptr);

private:
    GraphVertex *start;
    GraphVertex *end;
};

#endif // GRAPHEDGE_H
