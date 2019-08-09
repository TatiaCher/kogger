#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_centralWidget(new CentralWidget(this))
{
    setCentralWidget(m_centralWidget);
}

MainWindow::~MainWindow()
{
}

