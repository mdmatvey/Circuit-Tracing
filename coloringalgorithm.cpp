#include "coloringalgorithm.h"
#include <QSet>
#include <algorithm>

ColoringAlgorithm::ColoringAlgorithm(QObject *parent)
    : QObject(parent)
{
    initColorPalette();
}

void ColoringAlgorithm::initColorPalette()
{
    // Создаем палитру цветов для раскраски графа
    m_colorPalette = {
        QColor(220, 20, 60),   // Малиновый
        QColor(0, 128, 128),   // Бирюзовый
        QColor(255, 165, 0),   // Оранжевый
        QColor(106, 90, 205),  // Сине-фиолетовый
        QColor(50, 205, 50),   // Зеленый
        QColor(0, 191, 255),   // Глубокий голубой
        QColor(255, 215, 0),   // Золотой
        QColor(186, 85, 211),  // Пурпурный
        QColor(240, 128, 128), // Светло-коралловый
        QColor(0, 206, 209),   // Темно-бирюзовый
        QColor(60, 179, 113),  // Средний весенне-зеленый
        QColor(70, 130, 180),  // Стальной синий
        QColor(210, 105, 30),  // Шоколадный
        QColor(221, 160, 221), // Сиреневый
        QColor(154, 205, 50),  // Желто-зеленый
    };
}

int ColoringAlgorithm::applyGreedyColoring(QList<Vertex*> vertices)
{
    // Жадный алгоритм раскраски графа

    // Сбрасываем цвета вершин
    for (Vertex *vertex : vertices) {
        vertex->setColorIndex(-1);
    }

    int maxColor = 0;

    // Перебираем все вершины
    for (Vertex *vertex : vertices) {
        // Получаем множество использованных цветов у соседей
        QSet<int> usedColors;
        for (Vertex *neighbor : vertex->neighbors()) {
            if (neighbor->colorIndex() != -1) {
                usedColors.insert(neighbor->colorIndex());
            }
        }

        // Находим минимальный доступный цвет
        int colorIndex = 0;
        while (usedColors.contains(colorIndex)) {
            colorIndex++;
        }

        // Присваиваем цвет вершине
        vertex->setColorIndex(colorIndex);
        vertex->setColor(m_colorPalette[colorIndex % m_colorPalette.size()]);

        // Обновляем максимальный использованный цвет
        maxColor = std::max(maxColor, colorIndex);
    }

    return maxColor + 1; // Возвращаем количество использованных цветов
}
