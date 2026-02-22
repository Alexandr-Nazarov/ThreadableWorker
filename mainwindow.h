#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "mypoint.h"
#include "worker.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void paintEvent(QPaintEvent *event);

private:
    /**
     * @brief Меню
     */
    void createmenu();
    /**
     * @brief Сигналы-слоты
     */
    void connections();
    /**
     * @brief Создание объектов Worker
     */
    void createWorkers();
    /**
     * @brief Удаление Workers
     */
    void deleteWorkers();
    /**
     * @brief удалить потоки
     */
    void deleteThreads();
    /**
     * @brief запуск потоков
     */
    void startThreads();

private slots:
    /**
     * @brief Слот добавления точки
     * @param point
     */
    void slotAddPoint(MyPoint point);
    /**
     * @brief Очистить
     */
    void onClear();
    /**
     * @brief Слот команды запуска потоков
     */
    void slotQThread();

private:
    Ui::MainWindow *ui;
    QMenu            *menu_;
    QAction          *qthread_;
    QAction          *clear_;
    QVector<MyPoint> vPoints_;
    int              x_;            //общая координата для всех потоков
    QVector<Worker*> vWorkers_;
    QVector<QThread*>vThreads_;

};

#endif // MAINWINDOW_H
