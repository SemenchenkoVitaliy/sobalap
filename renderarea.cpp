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
//  if (!draw) return;
//  QPainter painter(this);

//  QPoint mousePos = mapFromGlobal(e->globalPos());
//  auto pShape = shapes.last();
//  painter.drawLine(pShape->currentPosition(), mousePos);
//  update();
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
