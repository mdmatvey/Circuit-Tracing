#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QCloseEvent>
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

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSaveAs_triggered();
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
    QString m_currentFilePath;

    void createActions();
    void updateModeButtons();

    bool saveGraph(const QString &filePath);
    bool loadGraph(const QString &filePath);
    bool maybeSave();
    void setCurrentFile(const QString &filePath);
    QString getFileDialogFilter() const;
};
#endif // MAINWINDOW_H
