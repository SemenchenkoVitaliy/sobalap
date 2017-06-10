#ifndef CALCULATEDIALOG_H_
#define CALCULATEDIALOG_H_

#include <QDialog>

namespace Ui {
class CalculateDialog;
}

class CalculateDialog : public QDialog {
  Q_OBJECT

 public:
  explicit CalculateDialog(QWidget *parent = 0);
  ~CalculateDialog();
  QString getVelocityInput() const;
  QString getDirectionInput() const;
  QString getPressureInput() const;
  void resetInput();

 private slots:
  void chooseVelocity();
  void choosePressure();

 private:
  Ui::CalculateDialog *ui;
};
#endif // CALCULATEDIALOG_H_
