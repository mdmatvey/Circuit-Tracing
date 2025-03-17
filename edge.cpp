#include "edge.h"
#include "vertex.h"

Edge::Edge(Vertex *sourceVertex, Vertex *destVertex, QObject *parent)
    : QObject(parent), m_sourceVertex(sourceVertex), m_destVertex(destVertex)
{
}
