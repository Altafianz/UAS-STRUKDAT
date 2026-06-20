#ifndef CREDITWINDOW_H
#define CREDITWINDOW_H

#include <QWidget>
class MainWindow;

namespace Ui {
class CreditWindow;
}

class CreditWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CreditWindow(MainWindow *mainWin = nullptr, QWidget *parent = nullptr);
    ~CreditWindow();

private slots:
    void on_backButton_clicked();

private:
    Ui::CreditWindow *ui;
    MainWindow *mainWindowRef;
};

#endif // CREDITWINDOW_H
