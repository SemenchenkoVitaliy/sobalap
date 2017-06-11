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
  void chooseWallPainter();
  void chooseInletPainter();
  void chooseOutletPainter();
  void choosePointPainter();

 signals:
  void redoEnabled(bool);
  void undoEnabled(bool);

 protected:
  void paintEvent(QPaintEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);

 private:
  QPainterPath *startPath(QPainterPath *prevPath, const QPoint &pos);

  enum ToolType {
    WALL_PAINTER, // 0
    INLET_PAINTER,
    OUTLET_PAINTER,
    POINT_PAINTER
  };
  QVector<QPainterPath *> shapes;
  QVector<QPainterPath *> redoStack;
  QPainterPath *inlet;
  QPainterPath *outlet;
  QPainterPath *trackingEllipse;
  QPoint trackingPoint;
  bool isDrawingWall;
  bool redoFlag;
  ToolType toolType;
};

#endif // RENDERAREA_H_
