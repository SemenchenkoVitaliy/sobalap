#include "calculatedialog.h"
#include "ui_calculatedialog.h"

CalculateDialog::CalculateDialog(QWidget *parent)
  : QDialog(parent), ui(new Ui::CalculateDialog) {
  ui->setupUi(this);
  this->setFixedSize(sizeHint());
}

CalculateDialog::~CalculateDialog() {
  delete ui;
}

QString CalculateDialog::getTimeInput() const {
  return ui->timeEdit->text();
}

QString CalculateDialog::getIntervalInput() const {
  return ui->intervalEdit->text();
}

void CalculateDialog::resetInput() {
  ui->timeEdit->clear();
  ui->intervalEdit->clear();
}
