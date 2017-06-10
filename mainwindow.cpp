#include "mainwindow.h"
#include <QGridLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QMouseEvent>
#include <QAction>
#include <QMenuBar>
#include <QMenu>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent) {
  QWidget *centralWidget = new QWidget(this);

  renderArea = new RenderArea(this);
  renderArea->setMinimumSize(600, 500);
  renderArea->setMaximumSize(600, 500);

  createActions();
  createMenu();

  QGridLayout *layout = new QGridLayout(this);
  layout->addWidget(renderArea);

  centralWidget->setLayout(layout);
  this->setCentralWidget(centralWidget);
}

MainWindow::~MainWindow() { }

void MainWindow::calculate() {
  // TODO: Call dialog with speed or pressure
  QMessageBox::information(this, "Test", "There is should be input data");
}

void MainWindow::createActions() {
  calculateAction = new QAction("Calculate", this);
  connect(calculateAction, SIGNAL(triggered()), this, SLOT(calculate()));
}

void MainWindow::createMenu() {
  palabosMenu = menuBar()->addMenu("Palabos");
  palabosMenu->addAction(calculateAction);
}

void MainWindow::mousePressEvent(QMouseEvent *e) {
  if (e->button() == Qt::LeftButton) {
    renderArea->onMousePress(e);
    e->accept();
  }
}

void MainWindow::mouseMoveEvent(QMouseEvent *e) {
  //renderArea->onMouseMove(e);
}

void MainWindow::keyPressEvent(QKeyEvent *e) {
  if (e->key() == Qt::Key_Space) {
    renderArea->closeShape();
  }
}
