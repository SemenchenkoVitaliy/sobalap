#ifndef RENDERAREA_H_
#define RENDERAREA_H_

#include <QWidget>
#include <QVector>
#include <QStack>
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
  void redo();

 signals:
  void redoEnabled(bool);
  void undoEnabled(bool);

 protected:
  void paintEvent(QPaintEvent *e);
  void mouseMoveEvent(QMouseEvent *e);

 private:
  QVector<QPainterPath *> shapes;
  QVector<QPainterPath *> redoStack;
  bool draw;
  bool redoFlag;
};

#endif // RENDERAREA_H_
