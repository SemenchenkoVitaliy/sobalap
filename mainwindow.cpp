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

  undoAction = new QAction("Undo", this);
  undoAction->setShortcut(QKeySequence::Undo);
  undoAction->setEnabled(false);
  connect(undoAction, SIGNAL(triggered()), renderArea, SLOT(undo()));
  connect(renderArea, SIGNAL(undoEnabled(bool)), undoAction, SLOT(setEnabled(bool)));

  redoAction = new QAction("Redo", this);
  redoAction->setShortcut(QKeySequence::Redo);
  redoAction->setEnabled(false);
  connect(redoAction, SIGNAL(triggered(bool)), renderArea, SLOT(redo()));
  connect(renderArea, SIGNAL(redoEnabled(bool)), redoAction, SLOT(setEnabled(bool)));
}

void MainWindow::createMenu() {
  palabosMenu = menuBar()->addMenu("Palabos");
  palabosMenu->addAction(calculateAction);

  editMenu = menuBar()->addMenu("Edit");
  editMenu->addAction(undoAction);
  editMenu->addAction(redoAction);
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
