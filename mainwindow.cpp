#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QStatusBar>
#include <QToolBar>
#include <QAction>
#include <QIcon>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Создаем граф и виджет для его отображения
    m_graph = new Graph(this);
    m_graphWidget = new GraphWidget(this);
    m_graphWidget->setGraph(m_graph);

    // Создаем действия
    createActions();

    // Размещаем виджет на главном окне
    setCentralWidget(m_graphWidget);

    // Подключаем сигналы
    connect(m_graphWidget, &GraphWidget::itemSelected,
            this, &MainWindow::handleItemSelected);
    connect(m_graph, &Graph::graphColored,
            this, &MainWindow::handleGraphColored);

    // Устанавливаем режим по умолчанию
    m_graphWidget->setEditMode(GraphEditMode::Select);
    updateModeButtons();

    // Начальное сообщение в строке состояния
    statusBar()->showMessage(tr("Ready. Select edit mode to start."));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createActions()
{
    // Создаем действия меню File
    ui->actionNew->setShortcut(QKeySequence::New);
    ui->actionExit->setShortcut(QKeySequence::Quit);
}

void MainWindow::updateModeButtons()
{
    // Обновляем состояние кнопок в зависимости от текущего режима
    GraphEditMode mode = m_graphWidget->editMode();

    ui->btnSelect->setChecked(mode == GraphEditMode::Select);
    ui->btnAddVertex->setChecked(mode == GraphEditMode::AddVertex);
    ui->btnAddEdge->setChecked(mode == GraphEditMode::AddEdge);
    ui->btnRemove->setChecked(mode == GraphEditMode::RemoveItem);
}

void MainWindow::on_actionNew_triggered()
{
    // Очищаем граф
    m_graph->clear();
    statusBar()->showMessage(tr("New graph created"));
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About Graph Coloring"),
                       tr("Graph Coloring for PCB Routing\n\n"
                          "This application demonstrates the graph coloring algorithm "
                          "for resolving conflicts in multilayer PCB routing.\n\n"
                          "© 2023 Your Name"));
}

void MainWindow::on_btnSelect_clicked()
{
    m_graphWidget->setEditMode(GraphEditMode::Select);
    updateModeButtons();
    statusBar()->showMessage(tr("Select mode: Click to select items"));
}

void MainWindow::on_btnAddVertex_clicked()
{
    m_graphWidget->setEditMode(GraphEditMode::AddVertex);
    updateModeButtons();
    statusBar()->showMessage(tr("Add Vertex mode: Click to add vertices"));
}

void MainWindow::on_btnAddEdge_clicked()
{
    m_graphWidget->setEditMode(GraphEditMode::AddEdge);
    updateModeButtons();
    statusBar()->showMessage(tr("Add Edge mode: Click two vertices to connect them"));
}

void MainWindow::on_btnRemove_clicked()
{
    m_graphWidget->setEditMode(GraphEditMode::RemoveItem);
    updateModeButtons();
    statusBar()->showMessage(tr("Remove mode: Click to remove vertices or edges"));
}

void MainWindow::on_btnColorGraph_clicked()
{
    // Запускаем алгоритм раскраски
    m_graphWidget->colorGraph();
    statusBar()->showMessage(tr("Graph coloring algorithm applied"));
}

void MainWindow::handleItemSelected(bool selected)
{
    if (selected) {
        statusBar()->showMessage(tr("Item selected. Press Delete to remove"));
    } else {
        updateModeButtons(); // Восстанавливаем сообщение о текущем режиме
    }
}

void MainWindow::handleGraphColored()
{
    // Выводим информацию о количестве использованных цветов
    int colorCount = m_graph->maxColorCount();
    statusBar()->showMessage(tr("Graph colored using %1 colors").arg(colorCount));

    // Можно вывести дополнительную информацию в диалоговом окне
    QMessageBox::information(this, tr("Graph Coloring Result"),
                             tr("The graph has been colored using %1 colors.\n\n"
                                "In PCB routing, this would require %1 layers to avoid conflicts.")
                                 .arg(colorCount));
}
