#ifndef RENDERAREA_H_
#define RENDERAREA_H_

#include <QWidget>
#include <QVector>
#include <QPainterPath>

class RenderArea : public QWidget {
  Q_OBJECT

 public:
  explicit RenderArea(QWidget *parent = 0);
  ~RenderArea();

 public slots:
  void onMousePress(QMouseEvent *e);
  void closeShape();
  void undo();

 protected:
  void paintEvent(QPaintEvent *e);
  void mouseMoveEvent(QMouseEvent *e);

 private:
  QVector<QPainterPath *> shapes;
  bool draw;
};

#endif // RENDERAREA_H_
