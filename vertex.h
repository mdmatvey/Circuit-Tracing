#ifndef VERTEX_H
#define VERTEX_H

#include <QObject>
#include <QPointF>
#include <QColor>
#include <QList>

class Edge;

// Класс Vertex представляет вершину графа
class Vertex : public QObject
{
    Q_OBJECT
public:
    explicit Vertex(const QPointF &position, QObject *parent = nullptr);

    QPointF position() const { return m_position; }
    void setPosition(const QPointF &position);

    QColor color() const { return m_color; }
    void setColor(const QColor &color);

    int colorIndex() const { return m_colorIndex; }
    void setColorIndex(int index) { m_colorIndex = index; }

    QList<Edge*> edges() const { return m_edges; }
    void addEdge(Edge *edge);
    void removeEdge(Edge *edge);

    // Получить соседние вершины
    QList<Vertex*> neighbors() const;

signals:
    void positionChanged();
    void colorChanged();

private:
    QPointF m_position;
    QColor m_color;
    int m_colorIndex;
    QList<Edge*> m_edges;
};

#endif // VERTEX_H
