#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <QWidget>

class MainWindow;

namespace Ui {
class EditorWindow;
}

class EditorWindow : public QWidget
{
    Q_OBJECT

public:
    explicit EditorWindow(MainWindow *mainWin = nullptr, QWidget *parent = nullptr);
    ~EditorWindow();

private slots:
    void on_backButton_clicked();

private:
    Ui::EditorWindow *ui;
    MainWindow *mainWindowRef;
};

#endif // EDITORWINDOW_H
