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
  QString getTimeInput() const;
  QString getIntervalInput() const;
  void resetInput();
  bool isVelocity() const;

 private:
  Ui::CalculateDialog *ui;
};
#endif // CALCULATEDIALOG_H_
