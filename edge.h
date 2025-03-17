#ifndef EDGE_H
#define EDGE_H

#include <QObject>

class Vertex;

// Класс Edge представляет ребро графа
class Edge : public QObject
{
    Q_OBJECT
public:
    explicit Edge(Vertex *sourceVertex, Vertex *destVertex, QObject *parent = nullptr);

    Vertex* sourceVertex() const { return m_sourceVertex; }
    Vertex* destVertex() const { return m_destVertex; }

private:
    Vertex *m_sourceVertex;
    Vertex *m_destVertex;
};

#endif // EDGE_H
