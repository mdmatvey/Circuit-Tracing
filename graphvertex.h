#ifndef GRAPHVERTEX_H
#define GRAPHVERTEX_H

#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <QBrush>
#include <QPen>
#include <QDebug>

class GraphVertex : public QGraphicsEllipseItem
{
public:
    // Конструктор для создания вершины
    GraphVertex(qreal x, qreal y, QGraphicsItem *parent = nullptr);

    // Устанавливаем ID вершины
    void setId(int id);

    // Получаем ID вершины
    int getId() const;

    // Проверяем, содержит ли вершина точку
    bool contains(const QPointF &point) const override;

protected:
    // Обрабатываем клик по вершине
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    int id; // Идентификатор вершины
};

#endif // GRAPHVERTEX_H
