#pragma once

#include <QMainWindow>
#include <QString>
#include "Viewer.hpp"



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow;}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui = nullptr;

private:
    void init();

protected:
    void dragEnterEvent(QDragEnterEvent *event);

    void dropEvent(QDropEvent *event);

private slots:
    void open_file();


private:
    void open_file(const QString &path);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};