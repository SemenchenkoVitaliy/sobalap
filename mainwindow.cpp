#include "mainwindow.h"
#include <QGridLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QMouseEvent>
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QDebug>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QFuture>

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

  if (!isResultCorrect()) return;
  double time = calculateDlg->getTimeInput().toDouble();
  double interval = calculateDlg->getIntervalInput().toDouble();
  bool velocityChecked = calculateDlg->isVelocity();
  ElementsMetaData data(*renderArea, time, interval, velocityChecked);

  Setup* palabosSetup = new Setup();
  palabosSetup->setInlet(data.getInletRect().topLeft.x,
                data.getInletRect().bottomRight.y,
                data.getInletRect().bottomRight.x,
                data.getInletRect().topLeft.y);
  palabosSetup->setOutlet(data.getOutletRect().topLeft.x,
                 data.getOutletRect().bottomRight.y,
                 data.getOutletRect().bottomRight.x,
                 data.getOutletRect().topLeft.y);
  palabosSetup->setCalcPoint(data.getTrackingPoint().x, data.getTrackingPoint().y);

  if(velocityChecked) {
      palabosSetup->setInletOutlet("velocity");
  } else {
      palabosSetup->setInletOutlet("pressure");
  }
  palabosSetup->setTimeInterval(data.getTime(), data.getInterval());
  palabosSetup->setSizes(8., 6.);
  palabosSetup->setOutDir(".\\tmp");
  palabosSetup->setOutFileName("result.gif");

  auto progress = this->createProgressDialog();

  auto resultWatcher = new QFutureWatcher<void>(this);
  connect(resultWatcher, SIGNAL(finished()), progress, SLOT(close()));
  connect(resultWatcher, SIGNAL(finished()), this, SLOT(onCalculationFinished()));

  QFuture<void> result = QtConcurrent::run([&palabosSetup, &data]() { palabosSetup->exec(data); });
  resultWatcher->setFuture(result);

  progress->exec();
  delete palabosSetup;
}

void MainWindow::onCalculationFinished() {
  QMessageBox::information(this, "Result", "Calculations ended");
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

bool MainWindow::isResultCorrect() {
  if (!renderArea->hasInlet()) {
    QMessageBox::critical(this, "Error",
                          "No inlet area. Please, set the inlet area");
    return false;
  } else if (!renderArea->hasOutlet()) {
    QMessageBox::critical(this, "Error",
                          "No outlet area. Please, set the outlet area");
    return false;
  } else if (!renderArea->hasTrackingPoint()) {
    QMessageBox::critical(this, "Error",
                          "No track point. Please, set the track point");
    return false;
  }

  double time = calculateDlg->getTimeInput().toDouble();
  if (!time) {
    QMessageBox::critical(this, "Error",
                          "Incorrect entered time");
    return false;
  }
  double interval = calculateDlg->getIntervalInput().toDouble();
  if (!interval || interval > time) {
    QMessageBox::critical(this, "Error",
                          "Incorrect entered interval");
    return false;
  }

  return true;
}

QProgressDialog *MainWindow::createProgressDialog() {
  auto *progress = new QProgressDialog(this);
  progress->setWindowTitle("Polabos");
  progress->setLabelText("Operation in progress. Please wait...");
  progress->setValue(0);
  progress->setMaximum(0);
  progress->setMinimum(0);
  progress->setModal(true);
  progress->setCancelButton(0);
  progress->setWindowFlags(progress->windowFlags() & ~Qt::WindowCloseButtonHint);
  return progress;
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
