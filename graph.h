#ifndef GRAPH_H
#define GRAPH_H

#include <QObject>
#include <QList>
#include <QPointF>
#include <QJsonObject>
#include <QJsonArray>
#include "vertex.h"
#include "edge.h"

class ColoringAlgorithm;

// Класс Graph представляет граф с вершинами и рёбрами
class Graph : public QObject
{
    Q_OBJECT
public:
    explicit Graph(QObject *parent = nullptr);
    ~Graph();

    // Управление вершинами и рёбрами
    Vertex* addVertex(const QPointF &position);
    Edge* addEdge(Vertex *source, Vertex *dest);
    void removeVertex(Vertex *vertex);
    void removeEdge(Edge *edge);

    // Доступ к данным
    QList<Vertex*> vertices() const { return m_vertices; }
    QList<Edge*> edges() const { return m_edges; }

    // Очистка графа
    void clear();

    // Раскраска графа жадным алгоритмом
    void colorVertices();

    // Получить максимальное количество цветов
    int maxColorCount() const { return m_maxColor; }

    // Поддержка JSON для сохранения/загрузки
    QJsonObject toJson() const;
    bool fromJson(const QJsonObject &json);

signals:
    void graphChanged();
    void vertexAdded(Vertex *vertex);
    void edgeAdded(Edge *edge);
    void vertexRemoved(Vertex *vertex);
    void edgeRemoved(Edge *edge);
    void graphColored();

private:
    QList<Vertex*> m_vertices;
    QList<Edge*> m_edges;
    int m_maxColor;  // Максимальный используемый цвет

    // Алгоритм раскраски
    ColoringAlgorithm *m_coloringAlgorithm;

    // Вспомогательный метод для поиска вершины по ID
    Vertex* findVertexById(int id) const;
};

#endif // GRAPH_H
