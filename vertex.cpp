#include "vertex.h"
#include "edge.h"

Vertex::Vertex(const QPointF &position, QObject *parent)
    : QObject(parent), m_position(position), m_color(Qt::white), m_colorIndex(-1)
{
}

void Vertex::setPosition(const QPointF &position)
{
    if (m_position != position) {
        m_position = position;
        emit positionChanged();
    }
}

void Vertex::setColor(const QColor &color)
{
    if (m_color != color) {
        m_color = color;
        emit colorChanged();
    }
}

void Vertex::addEdge(Edge *edge)
{
    if (!m_edges.contains(edge)) {
        m_edges.append(edge);
    }
}

void Vertex::removeEdge(Edge *edge)
{
    m_edges.removeOne(edge);
}

QList<Vertex*> Vertex::neighbors() const
{
    QList<Vertex*> result;
    for (Edge *edge : m_edges) {
        if (edge->sourceVertex() == this) {
            result.append(edge->destVertex());
        } else {
            result.append(edge->sourceVertex());
        }
    }
    return result;
}
