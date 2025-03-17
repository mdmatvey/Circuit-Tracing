#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include "graph.h"

// Графические элементы для отображения вершин и рёбер
class VertexItem : public QGraphicsEllipseItem
{
public:
    VertexItem(Vertex *vertex, QGraphicsItem *parent = nullptr);

    enum { Type = UserType + 1 };
    int type() const override { return Type; }

    Vertex* vertex() const { return m_vertex; }
    void updateColor();
    void updatePosition();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    Vertex *m_vertex;
    QPointF m_dragStartPosition;
};

class EdgeItem : public QGraphicsLineItem
{
public:
    EdgeItem(Edge *edge, QGraphicsItem *parent = nullptr);

    enum { Type = UserType + 2 };
    int type() const override { return Type; }

    Edge* edge() const { return m_edge; }
    void updatePosition();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    Edge *m_edge;
};

// Режимы работы с графом
enum class GraphEditMode {
    Select,
    AddVertex,
    AddEdge,
    RemoveItem
};

// Класс GraphWidget отвечает за визуализацию и редактирование графа
class GraphWidget : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphWidget(QWidget *parent = nullptr);
    ~GraphWidget();

    // Установка и получение графа
    void setGraph(Graph *graph);
    Graph* graph() const { return m_graph; }

    // Установка режима редактирования
    void setEditMode(GraphEditMode mode);
    GraphEditMode editMode() const { return m_editMode; }

    // Очистка графа
    void clearGraph();

    // Применение алгоритма раскраски
    void colorGraph();

signals:
    void vertexSelected(Vertex *vertex);
    void edgeSelected(Edge *edge);
    void itemSelected(bool selected);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void handleVertexAdded(Vertex *vertex);
    void handleEdgeAdded(Edge *edge);
    void handleVertexRemoved(Vertex *vertex);
    void handleEdgeRemoved(Edge *edge);
    void handleVertexPositionChanged();
    void handleVertexColorChanged();

private:
    Graph *m_graph;
    QGraphicsScene *m_scene;
    GraphEditMode m_editMode;
    Vertex *m_edgeStartVertex;
    QPointF m_edgeStartPoint;
    QGraphicsLineItem *m_tempEdgeLine;
    QMap<Vertex*, VertexItem*> m_vertexItems;
    QMap<Edge*, EdgeItem*> m_edgeItems;

    void setupGraph();
    void cleanupGraph();
    VertexItem* findVertexItemAt(const QPointF &pos);
};

#endif // GRAPHWIDGET_H
