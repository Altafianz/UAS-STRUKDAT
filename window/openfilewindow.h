#ifndef OPENFILEWINDOW_H
#define OPENFILEWINDOW_H

#include <QWidget>
class MainWindow;

namespace Ui {
class OpenFileWindow;
}

class OpenFileWindow : public QWidget
{
    Q_OBJECT

public:
    explicit OpenFileWindow(MainWindow *mainWin = nullptr, QWidget *parent = nullptr);
    ~OpenFileWindow();

private slots:                          
    void on_backButton_clicked(); 

private:
    Ui::OpenFileWindow *ui;
    MainWindow *mainWindowRef;
};

#endif // OPENFILEWINDOW_H
