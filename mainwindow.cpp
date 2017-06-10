#include "mainwindow.h"
#include <QGridLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QMouseEvent>
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent) {
  QWidget *centralWidget = new QWidget(this);

  renderArea = new RenderArea(this);
  renderArea->setMinimumSize(800, 600);
  renderArea->setMaximumSize(800, 600);

  calculateDlg = new CalculateDialog(this);

  createActions();
  createMenu();

  QGridLayout *layout = new QGridLayout(this);
  layout->addWidget(renderArea);

  centralWidget->setLayout(layout);
  this->setCentralWidget(centralWidget);
}

MainWindow::~MainWindow() { }

void MainWindow::calculate() {
  calculateDlg->resetInput();
  calculateDlg->exec();

  // TODO: check and send inputs to palabos, if dlg return OK
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
