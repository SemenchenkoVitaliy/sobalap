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
}

void RenderArea::onMousePress(QMouseEvent *e) {
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
  delete shapes.last();
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
