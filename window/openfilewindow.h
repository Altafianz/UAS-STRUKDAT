#ifndef OPENFILEWINDOW_H
#define OPENFILEWINDOW_H

#include <QWidget>
#include <QTableWidgetItem>
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
    void handleBackButtonClicked();
    void handleOpenButtonClicked();
    void onFileRowClicked(QTableWidgetItem *item);

private:
    Ui::OpenFileWindow *ui;
    MainWindow *mainWindowRef;
    QString selectedFilePath;

    void populateFileTable();
};

#endif // OPENFILEWINDOW_H