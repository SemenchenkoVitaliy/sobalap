#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QMainWindow>
#include "renderarea.h"
#include "calculatedialog.h"

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = 0);
  ~MainWindow();

 protected:
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void keyPressEvent(QKeyEvent *e);

 private slots:
  void calculate();

 private:
  void createActions();
  void createMenu();

  QMenu *palabosMenu;
  RenderArea *renderArea;
  QAction *calculateAction;
  CalculateDialog *calculateDlg;
};

#endif // MAINWINDOW_H_
