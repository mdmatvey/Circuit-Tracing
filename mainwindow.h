#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPushButton>
#include <QVector>
#include <QMouseEvent>
#include "graphvertex.h"
#include "graphedge.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void mousePressEvent(QMouseEvent *event) override; // Добавлена декларация

private slots:
    void onColorGraphClicked();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QVector<GraphVertex *> vertices;
    QVector<GraphEdge *> edges;
    int vertexCounter;
    GraphVertex *firstVertex;  // Первая выбранная вершина для создания ребра
    GraphVertex *secondVertex; // Вторая выбранная вершина
    void createEdge(GraphVertex *start, GraphVertex *end);
};

#endif // MAINWINDOW_H
