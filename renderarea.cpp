#include "renderarea.h"
#include <QPainter>
#include <QColor>
#include <QDebug>
#include <QBrush>
#include <QMouseEvent>

RenderArea::RenderArea(QWidget *parent)
  : QWidget(parent), isDrawingWall(false), redoFlag(false),
    toolType(WALL_PAINTER) {

}

RenderArea::~RenderArea() {
  foreach (auto shape, shapes) delete shape;
  foreach (auto shape, redoStack) delete shape;
}

bool RenderArea::hasInlet() const {
  return shapes.contains(inlet);
}

bool RenderArea::hasOutlet() const {
  return shapes.contains(outlet);
}

bool RenderArea::hasTrackingPoint() const {
  return shapes.contains(trackingEllipse);
}

void RenderArea::onMousePress(QMouseEvent *e) {
  redoStack.clear();
  redoFlag = false;
  emit redoEnabled(false);
  emit undoEnabled(true);

  QPoint mousePos = mapFromGlobal(e->globalPos());
  switch (toolType) {
    case WALL_PAINTER: {
      foreach (auto pShape, shapes) {
        if (pShape->contains(mousePos)) return;
      }
      if (!isDrawingWall) {
        isDrawingWall = true;
        shapes.push_back(new QPainterPath(mousePos));
        e->accept();
        return;
      }
      QPainterPath *shape = shapes.last();
      shape->lineTo(mousePos);
      break;
    }
    case INLET_PAINTER: {
      inlet = startPath(inlet, mousePos);
      if (!inlet) return;
      shapes.push_back(inlet);
      break;
    }
    case OUTLET_PAINTER: {
      outlet = startPath(outlet, mousePos);
      if (!outlet) return;
      shapes.push_back(outlet);
      break;
    }
    case POINT_PAINTER: {
      trackingEllipse = startPath(trackingEllipse, mousePos);
      if (!trackingEllipse) return;
      trackingEllipse->addEllipse(mousePos, 5, 5);
      shapes.push_back(trackingEllipse);
      trackingPoint = mousePos;
      break;
    }
  }
  update();
  e->accept();
}

void RenderArea::closeShape() {
  if (shapes.isEmpty()) return;
  shapes.last()->closeSubpath();
  isDrawingWall = false;
  update();
}

void RenderArea::undo() {
  if (shapes.isEmpty()) return;
  if (redoFlag) redoStack.clear();
  redoFlag = false;
  emit redoEnabled(true);

  auto pShape = shapes.takeLast();
  if (shapes.isEmpty()) emit undoEnabled(false);
  int shapeElementCount = pShape->elementCount();
  auto shapeElement = pShape->elementAt(shapeElementCount - 1);
  if (!shapeElement.isMoveTo()) pShape->closeSubpath();
  redoStack.push_back(pShape);
  isDrawingWall = false;
  update();
}

void RenderArea::redo() {
  if (redoStack.isEmpty()) return;
  redoFlag = true;
  auto pShape = redoStack.takeLast();
  if (redoStack.isEmpty()) emit redoEnabled(false);
  shapes.push_back(pShape);
  emit undoEnabled(true);
  isDrawingWall = false;
  update();
}

void RenderArea::chooseWallPainter() {
  toolType = WALL_PAINTER;
}

void RenderArea::chooseInletPainter() {
  toolType = INLET_PAINTER;
}

void RenderArea::chooseOutletPainter() {
  toolType = OUTLET_PAINTER;
}

void RenderArea::choosePointPainter() {
  toolType = POINT_PAINTER;
}

void RenderArea::paintEvent(QPaintEvent *e) {
  QColor workspaceColor(Qt::white);
  QPainter painter(this);
  painter.fillRect(this->rect(), workspaceColor);

  foreach (auto pShape, shapes) {
    if (pShape == inlet) {
      painter.setBrush(Qt::red);
    } else if (pShape == outlet) {
      painter.setBrush(Qt::yellow);
    } else if (pShape == trackingEllipse) {
      painter.setBrush(Qt::green);
    } else {
      painter.setBrush(Qt::blue);
    }
    painter.drawPath(*pShape);
  }

  e->accept();
}

void RenderArea::mouseMoveEvent(QMouseEvent *e) {
  // TODO: shadowy
}

void RenderArea::mouseReleaseEvent(QMouseEvent *e) {
  QPainterPath *currentShape;
  if (toolType == INLET_PAINTER) {
    currentShape = inlet;
  } else if (toolType == OUTLET_PAINTER) {
    currentShape = outlet;
  } else {
    return;
  }

  if (!currentShape) return;

  QPoint mousePos = mapFromGlobal(e->globalPos());
  int topLeftX = currentShape->elementAt(0).x;
  int topLeftY = currentShape->elementAt(0).y;
  QRect rect(QPoint(topLeftX, topLeftY), mousePos);

  foreach (auto pShape, shapes) {
    if (pShape == currentShape) continue;
    if (pShape->intersects(rect)) {
      if (currentShape) {
        shapes.removeOne(currentShape);
        if (toolType == INLET_PAINTER) {
          delete inlet;
          inlet = nullptr;
        } else {
          delete outlet;
          outlet = nullptr;
        }
      }
      return;
    }
  }
  currentShape->lineTo(rect.topRight());
  currentShape->lineTo(rect.bottomRight());
  currentShape->lineTo(rect.bottomLeft());
  currentShape->lineTo(rect.topLeft());
  update();
  e->accept();
}

QPainterPath *RenderArea::startPath(QPainterPath *prevPath, const QPoint &pos) {
  QPainterPath *temp = new QPainterPath(pos);
  foreach (auto pShape, shapes) {
    if (pShape->contains(pos)) {
      delete temp;
      temp = nullptr;
      break;
    }
  }
  if (prevPath) {
    shapes.removeOne(prevPath);
    delete prevPath;
  }
  return temp;
}


ElementsMetaData::ElementsMetaData(const RenderArea &renderArea, double time,
                                   double interval, bool velocityChecked) {
  this->velocityChecked = velocityChecked;
  this->time = time;
  this->interval = interval;
  this->areaSize = renderArea.size();
  foreach (auto pShape, renderArea.shapes) {
    if (pShape == renderArea.inlet) {
      inlet = validateRect(pShape);
    } else if (pShape == renderArea.outlet) {
      outlet = validateRect(pShape);
    } else if (pShape == renderArea.trackingEllipse) {
      setTrackingPoint(renderArea);
    } else {
      walls->addPath(*pShape);
    }
  }
}

bool ElementsMetaData::contains(int x, int y) {
  return walls->contains(QPoint(x, areaSize.height() - y));
}

double ElementsMetaData::getTime() const {
  return time;
}

double ElementsMetaData::getInterval() const {
  return interval;
}

ElementsMetaData::Point ElementsMetaData::getTrackingPoint() const {
  return {trackingPoint.x(), areaSize.height() - trackingPoint.y()};
}

ElementsMetaData::Rect ElementsMetaData::getInletRect() const {
  return convertRect(inlet);
}

ElementsMetaData::Rect ElementsMetaData::getOutletRect() const {
  return convertRect(outlet);
}

bool ElementsMetaData::isVelocityChecked() const {

}

void ElementsMetaData::setTrackingPoint(const RenderArea &area) {
  this->trackingPoint = area.trackingPoint;
  validatePoint(this->trackingPoint);
}


QRect ElementsMetaData::validateRect(const QPainterPath *path) {
  QPoint topLeft = ((QPointF) path->elementAt(0)).toPoint();
  validatePoint(topLeft);
  QPoint bottomRight = ((QPointF) path->elementAt(2)).toPoint();
  validatePoint(bottomRight);

  return QRect(topLeft, bottomRight);
}

void ElementsMetaData::validatePoint(QPoint &point) {
  if (point.x() < 0) {
    point.setX(0);
  } else if (point.x() >= areaSize.width()) {
    point.setX(areaSize.width() - 1);
  }

  if (point.y() < 0) {
    point.setY(0);
  } else if (point.y() >= areaSize.height()) {
    point.setY(areaSize.height() - 1);
  }
}

ElementsMetaData::Rect ElementsMetaData::convertRect(const QRect &rect) const {
  Point topLeft = {rect.topLeft().x(), areaSize.height() - rect.topLeft().y()};
  Point bottomRight = {rect.bottomRight().x(), areaSize.height() - rect.bottomRight().y()};
  return {topLeft, bottomRight};
}
