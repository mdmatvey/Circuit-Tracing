#include "graphwidget.h"
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QKeyEvent>
#include <QDebug>

// Константы для визуализации
constexpr int VERTEX_RADIUS = 15;
constexpr int EDGE_WIDTH = 2;
constexpr int SCENE_WIDTH = 800;
constexpr int SCENE_HEIGHT = 600;

// Реализация VertexItem

VertexItem::VertexItem(Vertex *vertex, QGraphicsItem *parent)
    : QGraphicsEllipseItem(-VERTEX_RADIUS, -VERTEX_RADIUS,
                           2 * VERTEX_RADIUS, 2 * VERTEX_RADIUS, parent),
    m_vertex(vertex)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setBrush(vertex->color());
    setPen(QPen(Qt::black, 1));
    setPos(vertex->position());
}

void VertexItem::updateColor()
{
    setBrush(m_vertex->color());
    update();
}

void VertexItem::updatePosition()
{
    setPos(m_vertex->position());
}

void VertexItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_dragStartPosition = pos();
    QGraphicsEllipseItem::mousePressEvent(event);
}

void VertexItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsEllipseItem::mouseMoveEvent(event);
    m_vertex->setPosition(pos());
}

void VertexItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsEllipseItem::mouseReleaseEvent(event);
}

void VertexItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsEllipseItem::paint(painter, option, widget);

    // Добавим номер цвета вершины
    if (m_vertex->colorIndex() >= 0) {
        painter->setPen(Qt::black);
        painter->drawText(boundingRect(), Qt::AlignCenter,
                          QString::number(m_vertex->colorIndex()));
    }
}

// Реализация EdgeItem

EdgeItem::EdgeItem(Edge *edge, QGraphicsItem *parent)
    : QGraphicsLineItem(parent), m_edge(edge)
{
    setPen(QPen(Qt::black, EDGE_WIDTH));
    setFlag(QGraphicsItem::ItemIsSelectable);
    updatePosition();
}

void EdgeItem::updatePosition()
{
    QLineF line(m_edge->sourceVertex()->position(),
                m_edge->destVertex()->position());
    setLine(line);
}

void EdgeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsLineItem::paint(painter, option, widget);
}

// Реализация GraphWidget

GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent), m_graph(nullptr), m_editMode(GraphEditMode::Select),
    m_edgeStartVertex(nullptr), m_tempEdgeLine(nullptr)
{
    // Настройка сцены
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, SCENE_WIDTH, SCENE_HEIGHT);
    m_scene->setBackgroundBrush(Qt::white);
    setScene(m_scene);

    // Настройка представления
    setRenderHint(QPainter::Antialiasing);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setCacheMode(QGraphicsView::CacheBackground);
    setDragMode(QGraphicsView::NoDrag);
}

GraphWidget::~GraphWidget()
{
    cleanupGraph();
    delete m_scene;
}

void GraphWidget::setGraph(Graph *graph)
{
    if (m_graph == graph)
        return;

    cleanupGraph();
    m_graph = graph;
    setupGraph();
}

void GraphWidget::setEditMode(GraphEditMode mode)
{
    m_editMode = mode;

    // Сбрасываем выделение при смене режима
    scene()->clearSelection();

    // Очищаем временное ребро, если оно есть
    if (m_tempEdgeLine) {
        scene()->removeItem(m_tempEdgeLine);
        delete m_tempEdgeLine;
        m_tempEdgeLine = nullptr;
    }

    m_edgeStartVertex = nullptr;

    // Устанавливаем курсор в зависимости от режима
    switch (m_editMode) {
    case GraphEditMode::Select:
        setCursor(Qt::ArrowCursor);
        break;
    case GraphEditMode::AddVertex:
        setCursor(Qt::CrossCursor);
        break;
    case GraphEditMode::AddEdge:
        setCursor(Qt::CrossCursor);
        break;
    case GraphEditMode::RemoveItem:
        setCursor(Qt::ForbiddenCursor);
        break;
    }
}

void GraphWidget::clearGraph()
{
    if (m_graph) {
        m_graph->clear();
    }
}

void GraphWidget::colorGraph()
{
    if (m_graph) {
        m_graph->colorVertices();
    }
}

void GraphWidget::mousePressEvent(QMouseEvent *event)
{
    QPointF scenePos = mapToScene(event->pos());

    switch (m_editMode) {
    case GraphEditMode::Select:
        QGraphicsView::mousePressEvent(event);
        break;

    case GraphEditMode::AddVertex:
        if (event->button() == Qt::LeftButton) {
            if (m_graph) {
                m_graph->addVertex(scenePos);
            }
        }
        break;

    case GraphEditMode::AddEdge:
        if (event->button() == Qt::LeftButton) {
            VertexItem *item = findVertexItemAt(scenePos);
            if (item) {
                if (!m_edgeStartVertex) {
                    // Первый клик - выбираем начальную вершину
                    m_edgeStartVertex = item->vertex();
                    m_edgeStartPoint = m_edgeStartVertex->position();

                    // Создаем временную линию
                    m_tempEdgeLine = new QGraphicsLineItem(
                        QLineF(m_edgeStartPoint, m_edgeStartPoint));
                    m_tempEdgeLine->setPen(QPen(Qt::gray, EDGE_WIDTH, Qt::DashLine));
                    m_scene->addItem(m_tempEdgeLine);
                } else {
                    // Второй клик - создаем ребро
                    if (m_graph && m_edgeStartVertex != item->vertex()) {
                        m_graph->addEdge(m_edgeStartVertex, item->vertex());
                    }

                    // Очищаем временное ребро
                    m_scene->removeItem(m_tempEdgeLine);
                    delete m_tempEdgeLine;
                    m_tempEdgeLine = nullptr;
                    m_edgeStartVertex = nullptr;
                }
            }
        } else if (event->button() == Qt::RightButton) {
            // Отменяем создание ребра при правом клике
            if (m_tempEdgeLine) {
                m_scene->removeItem(m_tempEdgeLine);
                delete m_tempEdgeLine;
                m_tempEdgeLine = nullptr;
            }
            m_edgeStartVertex = nullptr;
        }
        break;

    case GraphEditMode::RemoveItem:
        if (event->button() == Qt::LeftButton) {
            if (m_graph) {
                QGraphicsItem *item = m_scene->itemAt(scenePos, transform());
                if (item) {
                    if (item->type() == VertexItem::Type) {
                        m_graph->removeVertex(static_cast<VertexItem*>(item)->vertex());
                    } else if (item->type() == EdgeItem::Type) {
                        m_graph->removeEdge(static_cast<EdgeItem*>(item)->edge());
                    }
                }
            }
        }
        break;
    }
}

void GraphWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPointF scenePos = mapToScene(event->pos());

    if (m_editMode == GraphEditMode::AddEdge && m_tempEdgeLine) {
        // Обновляем конечную точку временной линии
        QLineF line(m_edgeStartPoint, scenePos);
        m_tempEdgeLine->setLine(line);
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void GraphWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);

    // Проверяем выделение
    bool hasSelection = !scene()->selectedItems().isEmpty();
    emit itemSelected(hasSelection);

    if (hasSelection) {
        QGraphicsItem *item = scene()->selectedItems().first();
        if (item->type() == VertexItem::Type) {
            emit vertexSelected(static_cast<VertexItem*>(item)->vertex());
        } else if (item->type() == EdgeItem::Type) {
            emit edgeSelected(static_cast<EdgeItem*>(item)->edge());
        }
    }
}

void GraphWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete) {
        if (m_graph) {
            for (QGraphicsItem *item : scene()->selectedItems()) {
                if (item->type() == VertexItem::Type) {
                    m_graph->removeVertex(static_cast<VertexItem*>(item)->vertex());
                } else if (item->type() == EdgeItem::Type) {
                    m_graph->removeEdge(static_cast<EdgeItem*>(item)->edge());
                }
            }
        }
    } else {
        QGraphicsView::keyPressEvent(event);
    }
}

void GraphWidget::handleVertexAdded(Vertex *vertex)
{
    if (!m_vertexItems.contains(vertex)) {
        VertexItem *item = new VertexItem(vertex);
        m_scene->addItem(item);
        m_vertexItems[vertex] = item;

        // Подключаем сигналы вершины
        connect(vertex, &Vertex::positionChanged, this, &GraphWidget::handleVertexPositionChanged);
        connect(vertex, &Vertex::colorChanged, this, &GraphWidget::handleVertexColorChanged);
    }
}

void GraphWidget::handleEdgeAdded(Edge *edge)
{
    if (!m_edgeItems.contains(edge)) {
        EdgeItem *item = new EdgeItem(edge);
        // Убедимся, что ребра отображаются под вершинами
        item->setZValue(-1);
        m_scene->addItem(item);
        m_edgeItems[edge] = item;
    }
}

void GraphWidget::handleVertexRemoved(Vertex *vertex)
{
    if (m_vertexItems.contains(vertex)) {
        VertexItem *item = m_vertexItems[vertex];
        m_scene->removeItem(item);
        m_vertexItems.remove(vertex);
        delete item;
    }
}

void GraphWidget::handleEdgeRemoved(Edge *edge)
{
    if (m_edgeItems.contains(edge)) {
        EdgeItem *item = m_edgeItems[edge];
        m_scene->removeItem(item);
        m_edgeItems.remove(edge);
        delete item;
    }
}

void GraphWidget::handleVertexPositionChanged()
{
    Vertex *vertex = qobject_cast<Vertex*>(sender());
    if (vertex && m_vertexItems.contains(vertex)) {
        m_vertexItems[vertex]->updatePosition();

        // Обновляем связанные ребра
        for (Edge *edge : vertex->edges()) {
            if (m_edgeItems.contains(edge)) {
                m_edgeItems[edge]->updatePosition();
            }
        }
    }
}

void GraphWidget::handleVertexColorChanged()
{
    Vertex *vertex = qobject_cast<Vertex*>(sender());
    if (vertex && m_vertexItems.contains(vertex)) {
        m_vertexItems[vertex]->updateColor();
    }
}

void GraphWidget::setupGraph()
{
    if (!m_graph)
        return;

    // Подключаем сигналы графа
    connect(m_graph, &Graph::vertexAdded, this, &GraphWidget::handleVertexAdded);
    connect(m_graph, &Graph::edgeAdded, this, &GraphWidget::handleEdgeAdded);
    connect(m_graph, &Graph::vertexRemoved, this, &GraphWidget::handleVertexRemoved);
    connect(m_graph, &Graph::edgeRemoved, this, &GraphWidget::handleEdgeRemoved);

    // Добавляем существующие вершины и ребра
    for (Vertex *vertex : m_graph->vertices()) {
        handleVertexAdded(vertex);
    }

    for (Edge *edge : m_graph->edges()) {
        handleEdgeAdded(edge);
    }
}

void GraphWidget::cleanupGraph()
{
    if (!m_graph)
        return;

    // Отключаем сигналы графа
    disconnect(m_graph, nullptr, this, nullptr);

    // Очищаем сцену
    for (auto it = m_vertexItems.begin(); it != m_vertexItems.end(); ++it) {
        m_scene->removeItem(it.value());
        delete it.value();
    }
    m_vertexItems.clear();

    for (auto it = m_edgeItems.begin(); it != m_edgeItems.end(); ++it) {
        m_scene->removeItem(it.value());
        delete it.value();
    }
    m_edgeItems.clear();

    m_graph = nullptr;
}

VertexItem* GraphWidget::findVertexItemAt(const QPointF &pos)
{
    QList<QGraphicsItem*> items = m_scene->items(pos);
    for (QGraphicsItem *item : items) {
        if (item->type() == VertexItem::Type) {
            return static_cast<VertexItem*>(item);
        }
    }
    return nullptr;
}
