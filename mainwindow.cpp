#include "mainwindow.h"
#include <QGridLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QMouseEvent>
#include <QAction>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent) {
  QWidget *centralWidget = new QWidget(this);

  renderArea = new RenderArea(this);
  renderArea->setMinimumSize(600, 500);
  renderArea->setMaximumSize(600, 500);

  QPushButton *calculateBtn = new QPushButton("Calculate", this);
  connect(calculateBtn, SIGNAL(released()), this, SLOT(calculate()));

  QGridLayout *layout = new QGridLayout(this);
  layout->addWidget(renderArea);
  layout->addWidget(calculateBtn);

  centralWidget->setLayout(layout);
  this->setCentralWidget(centralWidget);
}

MainWindow::~MainWindow() { }

void MainWindow::calculate() {
  // TODO: Call dialog with speed or pressure
  QMessageBox::information(this, "Test", "There is should be input data");
}

void MainWindow::mousePressEvent(QMouseEvent *e) {
  if (e->button() == Qt::LeftButton) {
    renderArea->onMousePress(e);
    e->accept();
  }
}

void MainWindow::mouseMoveEvent(QMouseEvent *e) {
  renderArea->onMouseMove(e);
}

void MainWindow::keyPressEvent(QKeyEvent *e) {
  if (e->key() == Qt::Key_Space) {
    renderArea->closeShape();
  }
}
