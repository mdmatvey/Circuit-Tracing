#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "graph.h"
#include "graphwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionNew_triggered();
    void on_actionExit_triggered();
    void on_actionAbout_triggered();

    void on_btnSelect_clicked();
    void on_btnAddVertex_clicked();
    void on_btnAddEdge_clicked();
    void on_btnRemove_clicked();
    void on_btnColorGraph_clicked();

    void handleItemSelected(bool selected);
    void handleGraphColored();

private:
    Ui::MainWindow *ui;
    Graph *m_graph;
    GraphWidget *m_graphWidget;

    void createActions();
    void updateModeButtons();
};
#endif // MAINWINDOW_H
