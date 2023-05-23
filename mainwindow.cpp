#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "sichuanrobot.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_pushButton_SichanRobot_clicked()
{
    if(!m_pSichuanRobot)
    {
        m_pSichuanRobot = new SichuanRobot;
    }

    this->hide();
    m_pSichuanRobot->show();
}
