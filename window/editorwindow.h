#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <QWidget>
#include <QListWidgetItem>
#include "../core/snapshot.h"
#include "../core/undoredo.h"
#include "../core/filemanager.h"

class MainWindow;

namespace Ui {
class EditorWindow;
}

class EditorWindow : public QWidget
{
    Q_OBJECT

public:
    explicit EditorWindow(MainWindow *mainWin = nullptr,
                           const QString &existingFilePath = QString(),
                           QWidget *parent = nullptr);
    ~EditorWindow();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void handleBackButton();
    void handleUndoButton();
    void handleRedoButton();
    void handleSaveButton();
    void toggleHistoryDropdown();
    void onHistoryItemClicked(QListWidgetItem *item);

    // logika save & unsaved-changes
    void onSaveAsConfirmed(const QString &fileName);
    void onSaveSuccessAck();
    void onExitWithSave();
    void onExitWithoutSave();

private:
    Ui::EditorWindow *ui;
    MainWindow *mainWindowRef;

    string       currentText;
    string       savedText;      // isi teks saat terakhir disimpan
    Stack        undoStack;
    Stack        redoStack;
    HistoryList  historyList;
    BoundedQueue historyQueue;
    string       filePath;
    bool         pendingCloseAfterSave = false;

    void refreshTextEdit(const string &text);
    void performSave();
    void writeToFile(const QString &path);
    void closeEditor();
    bool hasUnsavedChanges() const;
};

#endif // EDITORWINDOW_H