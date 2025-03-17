#include "graph.h"
#include "vertex.h"
#include "edge.h"
#include "coloringalgorithm.h"
#include <QRandomGenerator>

Graph::Graph(QObject *parent)
    : QObject(parent), m_maxColor(0)
{
    // Создаем алгоритм раскраски
    m_coloringAlgorithm = new ColoringAlgorithm(this);
}

Graph::~Graph()
{
    clear();
}

Vertex* Graph::addVertex(const QPointF &position)
{
    Vertex *vertex = new Vertex(position, this);
    m_vertices.append(vertex);
    emit vertexAdded(vertex);
    emit graphChanged();
    return vertex;
}

Edge* Graph::addEdge(Vertex *source, Vertex *dest)
{
    if (!source || !dest || source == dest)
        return nullptr;

    // Проверим, есть ли уже такое ребро
    for (Edge *edge : m_edges) {
        if ((edge->sourceVertex() == source && edge->destVertex() == dest) ||
            (edge->sourceVertex() == dest && edge->destVertex() == source)) {
            return nullptr;
        }
    }

    Edge *edge = new Edge(source, dest, this);
    m_edges.append(edge);
    source->addEdge(edge);
    dest->addEdge(edge);
    emit edgeAdded(edge);
    emit graphChanged();
    return edge;
}

void Graph::removeVertex(Vertex *vertex)
{
    if (!vertex)
        return;

    // Удаляем связанные ребра
    QList<Edge*> edgesToRemove = vertex->edges();
    for (Edge *edge : edgesToRemove) {
        removeEdge(edge);
    }

    m_vertices.removeOne(vertex);
    emit vertexRemoved(vertex);
    emit graphChanged();
    delete vertex;
}

void Graph::removeEdge(Edge *edge)
{
    if (!edge)
        return;

    edge->sourceVertex()->removeEdge(edge);
    edge->destVertex()->removeEdge(edge);
    m_edges.removeOne(edge);
    emit edgeRemoved(edge);
    emit graphChanged();
    delete edge;
}

void Graph::clear()
{
    while (!m_edges.isEmpty()) {
        removeEdge(m_edges.first());
    }

    while (!m_vertices.isEmpty()) {
        removeVertex(m_vertices.first());
    }

    m_maxColor = 0;
    emit graphChanged();
}

void Graph::colorVertices()
{
    // Применяем жадный алгоритм раскраски
    m_maxColor = m_coloringAlgorithm->applyGreedyColoring(m_vertices);
    emit graphColored();
}
