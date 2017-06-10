#include "calculatedialog.h"
#include "ui_calculatedialog.h"

CalculateDialog::CalculateDialog(QWidget *parent)
  : QDialog(parent), ui(new Ui::CalculateDialog) {
  ui->setupUi(this);
  connect(ui->pressureRadio, SIGNAL(pressed()), this, SLOT(choosePressure()));
  connect(ui->velocityRadio, SIGNAL(pressed()), this, SLOT(chooseVelocity()));
  this->setFixedSize(sizeHint());
}

CalculateDialog::~CalculateDialog() {
  delete ui;
}

void CalculateDialog::chooseVelocity() {
  ui->presureEdit->setEnabled(false);
  ui->velocityEdit->setEnabled(true);
}

void CalculateDialog::choosePressure() {
  ui->presureEdit->setEnabled(true);
  ui->velocityEdit->setEnabled(false);
}

QString CalculateDialog::getVelocityInput() const {
  return ui->velocityEdit->text();
}

QString CalculateDialog::getDirectionInput() const {
  return ui->directionEdit->text();
}

QString CalculateDialog::getPressureInput() const {
  return ui->presureEdit->text();
}

QString CalculateDialog::getTimeInput() const {
  return ui->timeEdit->text();
}

QString CalculateDialog::getIntervalInput() const {
  return ui->intervalEdit->text();
}

void CalculateDialog::resetInput() {
  ui->velocityEdit->clear();
  ui->presureEdit->clear();
  ui->directionEdit->clear();
  ui->timeEdit->clear();
  ui->intervalEdit->clear();
}
