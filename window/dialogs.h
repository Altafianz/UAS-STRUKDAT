#ifndef DIALOGS_H
#define DIALOGS_H

#include <QWidget>

namespace Ui {
class dialogs;
}

class dialogs : public QWidget
{
    Q_OBJECT

public:
    explicit dialogs(QWidget *parent = nullptr);
    ~dialogs();

private:
    Ui::dialogs *ui;
};

#endif // DIALOGS_H
