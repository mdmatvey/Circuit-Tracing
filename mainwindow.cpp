#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), scene(new QGraphicsScene(this)), vertexCounter(0), firstVertex(nullptr), secondVertex(nullptr)
{
    ui->setupUi(this);
    ui->graphicsView->setScene(scene);

    connect(ui->colorGraphButton, &QPushButton::clicked, this, &MainWindow::onColorGraphClicked);

    // Устанавливаем размеры сцены
    scene->setSceneRect(0, 0, 800, 600); // Можно подстроить под ваш интерфейс
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onColorGraphClicked()
{
    qDebug("Graph coloring triggered.");
}

void MainWindow::createEdge(GraphVertex *start, GraphVertex *end)
{
    // Создаем ребро между двумя вершинами
    auto *edge = new GraphEdge(start, end);
    scene->addItem(edge);
    edges.append(edge);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (ui->graphicsView->underMouse())
    {
        QPointF scenePos = ui->graphicsView->mapToScene(event->pos());

        // Логируем координаты клика
        qDebug() << "Click at:" << scenePos;

        // Проверяем, была ли выбрана первая вершина
        GraphVertex *selectedVertex = nullptr;
        for (GraphVertex *vertex : vertices)
        {
            if (vertex->contains(vertex->mapFromScene(scenePos)))
            {
                selectedVertex = vertex;
                qDebug() << "Selected vertex ID:" << vertex->getId();
                break;
            }
        }

        if (selectedVertex)
        {
            if (!firstVertex)
            {
                // Выбираем первую вершину
                firstVertex = selectedVertex;
                qDebug() << "First vertex selected, ID:" << firstVertex->getId();
            }
            else if (!secondVertex)
            {
                // Выбираем вторую вершину и создаём ребро
                secondVertex = selectedVertex;
                qDebug() << "Second vertex selected, ID:" << secondVertex->getId();

                createEdge(firstVertex, secondVertex);

                // Сброс состояния
                firstVertex = nullptr;
                secondVertex = nullptr;
            }
        }
        else
        {
            // Создаем новую вершину
            auto *vertex = new GraphVertex(scenePos.x(), scenePos.y());
            vertex->setId(vertexCounter++);
            scene->addItem(vertex);
            vertices.append(vertex);

            qDebug() << "Created new vertex with ID:" << vertex->getId();
        }
    }
}
