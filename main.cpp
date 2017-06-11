#include "mainwindow.h"
#include <QApplication>
#include "libr.h"

int main(int argc, char *argv[]) {
  Setup* obj = new Setup();
  obj->setInlet(0, 0, 20, 600);
  obj->setOutlet(780, 0, 800, 600);
  obj->setCalcPoint(40, 30);
  obj->setInletOutlet("pressure");
  obj->setTimeInterval(1.0, 0.05);
  obj->setSizes(8., 6.);
  obj->setOutDir(".\\tmp");
  obj->setOutFileName("result.gif");
  obj->exec();

  QApplication a(argc, argv);
  MainWindow w;
  w.show();

  return a.exec();
}
