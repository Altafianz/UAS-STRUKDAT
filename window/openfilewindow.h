#ifndef OPENFILEWINDOW_H
#define OPENFILEWINDOW_H

#include <QWidget>
#include <QTableWidgetItem>
#include <QDateTime>
class MainWindow;

namespace Ui {
class OpenFileWindow;
}

struct FileEntry {
    QString   name;
    QString   fullPath;
    QDateTime modified;
};

class OpenFileWindow : public QWidget
{
    Q_OBJECT

public:
    explicit OpenFileWindow(MainWindow *mainWin = nullptr, QWidget *parent = nullptr);
    ~OpenFileWindow();

private slots:
    void handleBackButtonClicked();
    void handleOpenButtonClicked();
    void handleSortByNameClicked();
    void handleSortByDateClicked();
    void onFileRowClicked(QTableWidgetItem *item);

private:
    Ui::OpenFileWindow *ui;
    MainWindow *mainWindowRef;
    QString selectedFilePath;

    QVector<FileEntry> fileEntries;
    bool sortNameAscending = true;
    bool sortDateAscending = true;

    void loadFileEntries();
    void renderFileTable();
    void bubbleSortByName(bool ascending); 
    void bubbleSortByDate(bool ascending); 
};

#endif // OPENFILEWINDOW_H