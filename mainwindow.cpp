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
  int dialogResult = calculateDlg->exec();
  if (dialogResult != QDialog::Accepted) return;

  if (!renderArea->hasInlet()) {
    QMessageBox::critical(this, "Error",
                          "No inlet area. Please, set the inlet area");
    return;
  } else if (!renderArea->hasOutlet()) {
    QMessageBox::critical(this, "Error",
                          "No outlet area. Please, set the outlet area");
    return;
  } else if (!renderArea->hasTrackingPoint()) {
    QMessageBox::critical(this, "Error",
                          "No track point. Please, set the track point");
    return;
  }

  double time = calculateDlg->getTimeInput().toDouble();
  if (!time) {
    QMessageBox::critical(this, "Error",
                          "Incorrect entered time");
    return;
  }
  double interval = calculateDlg->getIntervalInput().toDouble();
  if (!interval || interval > time) {
    QMessageBox::critical(this, "Error",
                          "Incorrect entered interval");
    return;
  }

  bool velocityChecked = calculateDlg->isVelocity();
  ElementsMetaData data(*renderArea, time, interval, velocityChecked);

  Setup* obj = new Setup();
  obj->setInlet(data.getInletRect().topLeft.x,
                data.getInletRect().bottomRight.y,
                data.getInletRect().bottomRight.x,
                data.getInletRect().topLeft.y);
  obj->setOutlet(data.getOutletRect().topLeft.x,
                 data.getOutletRect().bottomRight.y,
                 data.getOutletRect().bottomRight.x,
                 data.getOutletRect().topLeft.y);
  obj->setCalcPoint(data.getTrackingPoint().x, data.getTrackingPoint().y);
  if(velocityChecked)
      obj->setInletOutlet("velocity");
  else
      obj->setInletOutlet("pressure");
  obj->setTimeInterval(data.getTime(), data.getInterval());
  obj->setSizes(8., 6.);
  obj->setOutDir(".\\tmp");
  obj->setOutFileName("result.gif");
  obj->exec(data);

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

  wallPainterAction = new QAction("Walls painter", this);
  wallPainterAction->setCheckable(true);
  connect(wallPainterAction, SIGNAL(triggered()), renderArea, SLOT(chooseWallPainter()));
  connect(wallPainterAction, SIGNAL(triggered()), renderArea, SLOT(closeShape()));

  inletPainterAction = new QAction("Inlet painter", this);
  inletPainterAction->setCheckable(true);
  connect(inletPainterAction, SIGNAL(triggered()), renderArea, SLOT(chooseInletPainter()));
  connect(inletPainterAction, SIGNAL(triggered()), renderArea, SLOT(closeShape()));

  outletPainterAction = new QAction("Outlet painter", this);
  outletPainterAction->setCheckable(true);
  connect(outletPainterAction, SIGNAL(triggered()), renderArea, SLOT(chooseOutletPainter()));
  connect(outletPainterAction, SIGNAL(triggered()), renderArea, SLOT(closeShape()));

  trackingPointPainterAction = new QAction("Track point", this);
  trackingPointPainterAction->setCheckable(true);
  connect(trackingPointPainterAction, SIGNAL(triggered()), renderArea, SLOT(choosePointPainter()));
  connect(trackingPointPainterAction, SIGNAL(triggered()), renderArea, SLOT(closeShape()));

  toolsActionGroup = new QActionGroup(this);
  toolsActionGroup->addAction(wallPainterAction);
  toolsActionGroup->addAction(inletPainterAction);
  toolsActionGroup->addAction(outletPainterAction);
  toolsActionGroup->addAction(trackingPointPainterAction);
  wallPainterAction->setChecked(true);
}

void MainWindow::createMenu() {
  palabosMenu = menuBar()->addMenu("Palabos");
  palabosMenu->addAction(calculateAction);

  editMenu = menuBar()->addMenu("Edit");
  editMenu->addAction(undoAction);
  editMenu->addAction(redoAction);

  toolsMenu = menuBar()->addMenu("Tools");
  toolsMenu->addAction(wallPainterAction);
  toolsMenu->addAction(inletPainterAction);
  toolsMenu->addAction(outletPainterAction);
  toolsMenu->addAction(trackingPointPainterAction);
}

void MainWindow::mousePressEvent(QMouseEvent *e) {
  if (e->button() == Qt::LeftButton) {
    renderArea->onMousePress(e);
    e->accept();
  }
}

void MainWindow::keyPressEvent(QKeyEvent *e) {
  if (e->key() == Qt::Key_Space) {
    renderArea->closeShape();
  }
}
