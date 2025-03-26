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
#include <QFileDialog>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCloseEvent>
#include <QFileInfo>

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
    statusBar()->showMessage(tr("Готово. Выберите режим редактирования, чтобы начать."));

    // Устанавливаем заголовок по умолчанию
    setCurrentFile("");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::createActions()
{
    // Создаем действия меню File
    ui->actionNew->setShortcut(QKeySequence::New);
    ui->actionOpen->setShortcut(QKeySequence::Open);
    ui->actionSave->setShortcut(QKeySequence::Save);
    ui->actionSaveAs->setShortcut(QKeySequence::SaveAs);
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
    if (maybeSave()) {
        // Очищаем граф
        m_graph->clear();
        setCurrentFile("");
        statusBar()->showMessage(tr("Новый граф создан"));
    }
}

void MainWindow::on_actionOpen_triggered()
{
    if (maybeSave()) {
        QString filePath = QFileDialog::getOpenFileName(this,
                                                        tr("Открыть граф"), "", getFileDialogFilter());
        if (!filePath.isEmpty()) {
            if (loadGraph(filePath)) {
                setCurrentFile(filePath);
                statusBar()->showMessage(tr("Граф загружен из %1").arg(filePath));
            }
        }
    }
}

void MainWindow::on_actionSave_triggered()
{
    if (m_currentFilePath.isEmpty()) {
        on_actionSaveAs_triggered();
    } else {
        if (saveGraph(m_currentFilePath)) {
            statusBar()->showMessage(tr("Граф сохранен в %1").arg(m_currentFilePath));
        }
    }
}

void MainWindow::on_actionSaveAs_triggered()
{
    QString filePath = QFileDialog::getSaveFileName(this,
                                                    tr("Сохранить граф"), "", getFileDialogFilter());
    if (!filePath.isEmpty()) {
        if (saveGraph(filePath)) {
            setCurrentFile(filePath);
            statusBar()->showMessage(tr("Граф сохранен в %1").arg(filePath));
        }
    }
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About Graph Coloring"),
                       tr("Раскраска графа для трассировки печатной платы\n\n"
                          "Это приложение демонстрирует алгоритм раскраски графа. "
                          "для разрешения конфликтов при разводке многослойных печатных плат.\n\n"
                          "© 2025 ст. гр. 1214 Лёшин М.Д."));
}

void MainWindow::on_btnSelect_clicked()
{
    m_graphWidget->setEditMode(GraphEditMode::Select);
    updateModeButtons();
    statusBar()->showMessage(tr("Режим выбора: нажмите, чтобы выбрать элементы"));
}

void MainWindow::on_btnAddVertex_clicked()
{
    m_graphWidget->setEditMode(GraphEditMode::AddVertex);
    updateModeButtons();
    statusBar()->showMessage(tr("Режим добавления вершин: нажмите, чтобы добавить вершины."));
}

void MainWindow::on_btnAddEdge_clicked()
{
    m_graphWidget->setEditMode(GraphEditMode::AddEdge);
    updateModeButtons();
    statusBar()->showMessage(tr("Режим добавления ребра: щелкните две вершины, чтобы соединить их."));
}

void MainWindow::on_btnRemove_clicked()
{
    m_graphWidget->setEditMode(GraphEditMode::RemoveItem);
    updateModeButtons();
    statusBar()->showMessage(tr("Режим удаления: нажмите, чтобы удалить вершины или ребра."));
}

void MainWindow::on_btnColorGraph_clicked()
{
    // Запускаем алгоритм раскраски
    m_graphWidget->colorGraph();
    statusBar()->showMessage(tr("Применен алгоритм раскраски графа"));
}

void MainWindow::handleItemSelected(bool selected)
{
    if (selected) {
        statusBar()->showMessage(tr("Выбран элемент. Нажмите Delete, чтобы удалить"));
    } else {
        updateModeButtons(); // Восстанавливаем сообщение о текущем режиме
    }
}

void MainWindow::handleGraphColored()
{
    // Выводим информацию о количестве использованных цветов
    int colorCount = m_graph->maxColorCount();
    statusBar()->showMessage(tr("Граф раскрашен, используя %1 цвета(-ов)").arg(colorCount));

    // Можно вывести дополнительную информацию в диалоговом окне
    QMessageBox::information(this, tr("Результат раскраски графа"),
                             tr("Граф раскрашен, используя %1 цвета(-ов).\n\n"
                                "При разводке печатной платы это потребует %1 слоя(-ев), чтобы избежать конфликтов.")
                                 .arg(colorCount));
}

bool MainWindow::saveGraph(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("Ошибка"),
                             tr("Не удалось сохранить файл %1:\n%2.").arg(filePath).arg(file.errorString()));
        return false;
    }

    QJsonObject jsonGraph = m_graph->toJson();
    QJsonDocument doc(jsonGraph);
    file.write(doc.toJson());

    return true;
}

bool MainWindow::loadGraph(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Error"),
                             tr("Не удалось открыть файл %1:\n%2.").arg(filePath).arg(file.errorString()));
        return false;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (doc.isNull() || !doc.isObject()) {
        QMessageBox::warning(this, tr("Ошибка"),
                             tr("Неправильный формат JSON в файле %1.").arg(filePath));
        return false;
    }

    if (!m_graph->fromJson(doc.object())) {
        QMessageBox::warning(this, tr("Ошибка"),
                             tr("Ошибка загрузки графа из file %1.").arg(filePath));
        return false;
    }

    return true;
}

bool MainWindow::maybeSave()
{
    // Если граф не пуст, предлагаем сохранить изменения
    if (!m_graph->vertices().isEmpty()) {
        QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("Раскраска графа"),
                                                               tr("График был изменен.\nВы хотите сохранить изменения?"),
                                                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (ret == QMessageBox::Save) {
            on_actionSave_triggered();
            return true;
        } else if (ret == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

void MainWindow::setCurrentFile(const QString &filePath)
{
    m_currentFilePath = filePath;

    QString title = tr("Раскраска графа для трассировки печатной платы");
    if (!m_currentFilePath.isEmpty()) {
        title += " - " + QFileInfo(m_currentFilePath).fileName();
    }

    setWindowTitle(title);
}

QString MainWindow::getFileDialogFilter() const
{
    return tr("JSON файлы (*.json);;Все файлы (*)");
}
