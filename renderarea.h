#ifndef RENDERAREA_H_
#define RENDERAREA_H_

#include <QWidget>
#include <QVector>
#include <QStack>
#include <QPainterPath>

class ElementsMetaData;

class RenderArea : public QWidget {
  Q_OBJECT

 public:
  explicit RenderArea(QWidget *parent = 0);
  ~RenderArea();
  bool hasInlet() const;
  bool hasOutlet() const;
  bool hasTrackingPoint() const;

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

  friend ElementsMetaData;
};

class ElementsMetaData {
 public:

  struct Point {
    int x;
    int y;
  };

  struct Rect {
    Point topLeft;
    Point bottomRight;
  };

  ElementsMetaData(const RenderArea &renderArea, double time,
                   double interval, bool velocityChecked);
  bool contains(int x, int y);
  double getTime() const;
  double getInterval() const;
  Point getTrackingPoint() const;
  Rect getInletRect() const;
  Rect getOutletRect() const;
  bool isVelocityChecked() const;

 private:
  void setTrackingPoint(const RenderArea &area);
  QRect validateRect(const QPainterPath *path);
  void validatePoint(QPoint &point);
  Rect convertRect(const QRect &rect) const;

  QPainterPath *walls;
  QRect inlet;
  QRect outlet;
  QPoint trackingPoint;
  double time;
  double interval;
  QSize areaSize;
  bool velocityChecked;
};

#endif // RENDERAREA_H_
