#include "renderarea.h"
#include <QPainter>
#include <QColor>
#include <QDebug>
#include <QBrush>
#include <QMouseEvent>

RenderArea::RenderArea(QWidget *parent)
  : QWidget(parent), draw(false) {

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
  if (!draw) {
    draw = true;
    shapes.push_back(new QPainterPath(mousePos));
    e->accept();
    return;
  }
  QPainterPath *shape = shapes.last();
  shape->lineTo(mousePos);
  update();
  e->accept();
}

void RenderArea::closeShape() {
  if (shapes.isEmpty()) return;
  shapes.last()->closeSubpath();
  draw = false;
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
  draw = false;
  update();
}

void RenderArea::redo() {
  if (redoStack.isEmpty()) return;
  redoFlag = true;
  auto pShape = redoStack.takeLast();
  if (redoStack.isEmpty()) emit redoEnabled(false);
  shapes.push_back(pShape);
  emit undoEnabled(true);
  draw = false;
  update();
}

void RenderArea::paintEvent(QPaintEvent *e) {
  QColor workspaceColor(Qt::white);
  QPainter painter(this);
  painter.fillRect(this->rect(), workspaceColor);

  painter.setBrush(Qt::green);
  foreach (auto pShape, shapes) {
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
