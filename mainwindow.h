#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QMainWindow>
#include <QActionGroup>
#include "renderarea.h"
#include "calculatedialog.h"
#include "libr.h"

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = 0);
  ~MainWindow();

 protected:
  void mousePressEvent(QMouseEvent *e);
  void keyPressEvent(QKeyEvent *e);

 private slots:
  void calculate();

 private:
  void createActions();
  void createMenu();

  QMenu *palabosMenu;
  QMenu *editMenu;
  QMenu *toolsMenu;
  QActionGroup *toolsActionGroup;
  QAction *wallPainterAction;
  QAction *inletPainterAction;
  QAction *outletPainterAction;
  QAction *trackingPointPainterAction;
  QAction *calculateAction;
  QAction *undoAction;
  QAction *redoAction;
  RenderArea *renderArea;
  CalculateDialog *calculateDlg;
};

#endif // MAINWINDOW_H_
