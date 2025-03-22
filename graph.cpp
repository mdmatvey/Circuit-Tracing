#include "graph.h"
#include "vertex.h"
#include "edge.h"
#include "coloringalgorithm.h"
#include <QRandomGenerator>
#include <QJsonArray>
#include <QJsonObject>
#include <QMap>

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

QJsonObject Graph::toJson() const
{
    QJsonObject graphJson;
    QJsonArray verticesJson;
    QJsonArray edgesJson;

    // Сохраняем вершины с уникальными ID
    QMap<Vertex*, int> vertexToId;
    int nextId = 0;

    for (Vertex *vertex : m_vertices) {
        QJsonObject vertexJson;
        vertexJson["id"] = nextId;
        vertexJson["x"] = vertex->position().x();
        vertexJson["y"] = vertex->position().y();
        vertexJson["color_index"] = vertex->colorIndex();

        verticesJson.append(vertexJson);
        vertexToId[vertex] = nextId;
        nextId++;
    }

    // Сохраняем ребра, ссылаясь на ID вершин
    for (Edge *edge : m_edges) {
        QJsonObject edgeJson;
        edgeJson["source_id"] = vertexToId[edge->sourceVertex()];
        edgeJson["dest_id"] = vertexToId[edge->destVertex()];

        edgesJson.append(edgeJson);
    }

    graphJson["vertices"] = verticesJson;
    graphJson["edges"] = edgesJson;
    graphJson["max_color"] = m_maxColor;

    return graphJson;
}

bool Graph::fromJson(const QJsonObject &json)
{
    // Очищаем текущий граф
    clear();

    // Проверяем наличие необходимых ключей
    if (!json.contains("vertices") || !json.contains("edges")) {
        return false;
    }

    QJsonArray verticesJson = json["vertices"].toArray();
    QJsonArray edgesJson = json["edges"].toArray();

    // Загружаем вершины
    QMap<int, Vertex*> idToVertex;

    for (const QJsonValue &vertexValue : verticesJson) {
        QJsonObject vertexJson = vertexValue.toObject();
        int id = vertexJson["id"].toInt();
        double x = vertexJson["x"].toDouble();
        double y = vertexJson["y"].toDouble();
        int colorIndex = vertexJson["color_index"].toInt(-1);

        Vertex *vertex = addVertex(QPointF(x, y));
        vertex->setColorIndex(colorIndex);

        // Если у вершины указан цвет, устанавливаем его
        if (colorIndex >= 0 && m_coloringAlgorithm) {
            QVector<QColor> palette = m_coloringAlgorithm->colorPalette();
            if (colorIndex < palette.size()) {
                vertex->setColor(palette[colorIndex]);
            }
        }

        idToVertex[id] = vertex;
    }

    // Загружаем ребра
    for (const QJsonValue &edgeValue : edgesJson) {
        QJsonObject edgeJson = edgeValue.toObject();
        int sourceId = edgeJson["source_id"].toInt();
        int destId = edgeJson["dest_id"].toInt();

        if (idToVertex.contains(sourceId) && idToVertex.contains(destId)) {
            addEdge(idToVertex[sourceId], idToVertex[destId]);
        }
    }

    // Загружаем максимальный цвет, если он есть
    if (json.contains("max_color")) {
        m_maxColor = json["max_color"].toInt();
    }

    emit graphChanged();
    return true;
}

Vertex* Graph::findVertexById(int id) const
{
    if (id >= 0 && id < m_vertices.size()) {
        return m_vertices[id];
    }
    return nullptr;
}
