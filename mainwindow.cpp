#include <QObject>
#include "mainwindow.h"
#include "ui_mainwindow.h"

int THREADSCOUNT = QThread::idealThreadCount() + 1; //количество потоков
//-------------------------------------------------------------------
//
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    menu_(new QMenu(this)),
    qthread_(new QAction("QThread", this)),
    clear_(new QAction("Clear", this))
{
    ui->setupUi(this);

     qRegisterMetaType<MyPoint>("MyPoint");     // зарегистрируем MyPoint тип
    createmenu();
    connections();
}

//-------------------------------------------------------------------
//
MainWindow::~MainWindow()
{
    deleteThreads();
    deleteWorkers();
    delete ui;
}

//-------------------------------------------------------------------
//
void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    for (const MyPoint &point : vPoints_)
    {
      point.draw(painter);
    }
}

//-------------------------------------------------------------------
//
void MainWindow::createmenu()
{
    this->setMinimumSize(QSize(wk::COUNT * THREADSCOUNT + 20, 100 + (THREADSCOUNT + 1) * 30 + ui->threadsCount->height() * 2));
    menu_ = menuBar()->addMenu("Commands");
    menu_->addAction(qthread_);
    menu_->addAction(clear_);

    ui->threadsCount->setText("Ideal Thread Count: " + QString::number(THREADSCOUNT));
}

//-------------------------------------------------------------------
//
void MainWindow::connections()
{

    connect(clear_, &QAction::triggered, this, &MainWindow::onClear);
    connect(qthread_, &QAction::triggered, this, &MainWindow::slotQThread);

}

//-------------------------------------------------------------------
//
void MainWindow::createWorkers()
{

    //qRegisterMetaType<MyPoint>("MyPoint");     // зарегистрируем MyPoint тип

    for (int i = 0; i < THREADSCOUNT; ++i)
    {
        Worker *worker = new Worker(&x_, wk::YSTART + i *30, Qt::GlobalColor(Qt::red + i % (Qt::transparent - Qt::red)), wk::COUNT);
        connect (worker, &Worker::signalAddPoint, this, &MainWindow::slotAddPoint, Qt::QueuedConnection); //Qt::QueuedConnection т.к в разных потоках; и надо зарегистрировать MyPoint
        vWorkers_.append(worker);
    }
}

//-------------------------------------------------------------------
//
void MainWindow::deleteWorkers()
{
    if (!vWorkers_.isEmpty())
    {
        for (Worker* worker : vWorkers_)
        {
            if (worker)
            {
                delete worker;
            }
        }
        vWorkers_.clear();
    }
}

//-------------------------------------------------------------------
//
void MainWindow::deleteThreads()
{
    if (!vThreads_.isEmpty())
    {
        for (QThread* thread : vThreads_)
        {
            if (thread && thread->isRunning())
            {
                thread->quit();
                thread->wait();
            }
        }
        vThreads_.clear();
    }
}

//-------------------------------------------------------------------
//
void MainWindow::slotAddPoint(MyPoint point)
{
    vPoints_.append(point);
    this->repaint();
}

//-------------------------------------------------------------------
//
void MainWindow::onClear()
{
    vPoints_.clear();

    deleteThreads();
    deleteWorkers();

    this->repaint();
}

void MainWindow::slotQThread()
{
    onClear();
    x_ = 10;
    createWorkers();
    for (Worker* worker : vWorkers_)
    {
        QThread *thread = new QThread(this);
        vThreads_.append(thread);
        worker->moveToThread(thread);
        connect(thread, &QThread::started, worker, &Worker::doWork);
        connect(worker, &Worker::endWork, thread, &QThread::quit);
        connect(thread, &QThread::finished, worker, &Worker::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        connect(thread, &QThread::finished, this, &MainWindow::onDelete);
    }
    startThreads();
}

void MainWindow::onDelete()
{
    vWorkers_.clear();
    vThreads_.clear();
}

//-------------------------------------------------------------------
//
void MainWindow::startThreads()
{
    for (QThread* thread : vThreads_)
    {
        thread->start();
    }
}

