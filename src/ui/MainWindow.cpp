#include "ui/MainWindow.hpp"
#include "./ui_MainWindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>
#include "io/PLYParser.hpp"

 

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), _viewer(new Viewer(this))
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    setAcceptDrops(true);

    ui->horizontalLayout->addWidget(_viewer);

    QObject::connect(ui->actionshow_point, &QAction::triggered, _viewer, &Viewer::set_show_point);
    QObject::connect(ui->actionshow_surface, &QAction::triggered, _viewer, &Viewer::set_show_surface);
    QObject::connect(ui->actionshow_edge, &QAction::triggered, _viewer, &Viewer::set_show_edge);
}





void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QString suffixs = "pcd PCD ply PLY xyz XYZ";
    QFileInfo file_info(event->mimeData()->urls().front().toLocalFile());
    if( file_info.isFile() && suffixs.contains(file_info.suffix()))
    {
        open_file(file_info.absoluteFilePath());
    }
}



void MainWindow::open_file()
{
    QFileDialog *dialog = new QFileDialog();
    dialog->setModal(true);
    dialog->setFileMode(QFileDialog::ExistingFile);
    QString path = dialog->getOpenFileName(dialog, nullptr, QString(), "All Files: (*.*);;PLY: (*.ply *.PLY);;PCD: (*.pcd *.PCD);;");
    open_file(path);
    delete dialog;
}



void MainWindow::open_file(const QString &path)
{
    if (!QFileInfo(path).isFile())
    {
        return;
    }

    PointCloud::PointCloud pd;

    if (path.endsWith(".ply") || path.endsWith(".PLY"))
    {
        std::ifstream file(path.toLocal8Bit(), std::ios_base::in);
        PLYParser::parse(file, pd);
        file.close();
    }

    _viewer->load_data(pd);
}