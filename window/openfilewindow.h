#ifndef OPENFILEWINDOW_H
#define OPENFILEWINDOW_H

#include <QWidget>

namespace Ui {
class OpenFileWindow;
}

class OpenFileWindow : public QWidget
{
    Q_OBJECT

public:
    explicit OpenFileWindow(QWidget *parent = nullptr);
    ~OpenFileWindow();

private:
    Ui::OpenFileWindow *ui;
};

#endif // OPENFILEWINDOW_H
