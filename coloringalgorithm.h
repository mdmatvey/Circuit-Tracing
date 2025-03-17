#ifndef COLORINGALGORITHM_H
#define COLORINGALGORITHM_H

#include <QObject>
#include <QList>
#include <QVector>
#include <QColor>
#include "vertex.h"

// Класс ColoringAlgorithm реализует алгоритм раскраски графа
class ColoringAlgorithm : public QObject
{
    Q_OBJECT
public:
    explicit ColoringAlgorithm(QObject *parent = nullptr);

    // Применить жадный алгоритм раскраски к списку вершин
    int applyGreedyColoring(QList<Vertex*> vertices);

    // Получить палитру цветов
    QVector<QColor> colorPalette() const { return m_colorPalette; }

private:
    // Палитра доступных цветов для раскраски
    QVector<QColor> m_colorPalette;
    void initColorPalette();
};

#endif // COLORINGALGORITHM_H
