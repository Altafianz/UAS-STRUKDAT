#include "editorwindow.h"
#include "ui_editorwindow.h"
#include "mainwindow.h"
#include "dialogs.h"
#include <QKeyEvent>
#include <QTimer>
#include <QFileInfo>

EditorWindow::EditorWindow(MainWindow *mainWin, const QString &existingFilePath, QWidget *parent)
    : QWidget(parent), ui(new Ui::EditorWindow), mainWindowRef(mainWin)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    initStack(undoStack);
    initStack(redoStack);
    initHistoryList(historyList);
    initQueue(historyQueue);

    if (!existingFilePath.isEmpty()) {
        // dibuka dari file yang sudah ada -> load isinya
        filePath    = existingFilePath.toStdString();
        currentText = loadFile(filePath);

        QFileInfo info(existingFilePath);
        ui->labelNamaFile->setText("File: " + info.fileName());
    } else {
        currentText = "";
    }

    savedText = currentText; // baseline buat deteksi perubahan
    appendHistory(historyList, currentText);
    refreshTextEdit(currentText);

    ui->plainTextEdit->installEventFilter(this);
    ui->historyListWidget->hide();

    connect(ui->backButton, &QPushButton::clicked, this, &EditorWindow::handleBackButton);
    connect(ui->undoButton, &QPushButton::clicked, this, &EditorWindow::handleUndoButton);
    connect(ui->redoButton, &QPushButton::clicked, this, &EditorWindow::handleRedoButton);
    connect(ui->saveButton, &QPushButton::clicked, this, &EditorWindow::handleSaveButton);
    connect(ui->dropDown,   &QPushButton::clicked, this, &EditorWindow::toggleHistoryDropdown);
    connect(ui->historyListWidget, &QListWidget::itemClicked,
            this, &EditorWindow::onHistoryItemClicked);
}

EditorWindow::~EditorWindow()
{
    delete ui;
}

bool EditorWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->plainTextEdit && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        if (keyEvent->key() == Qt::Key_Space) {
            QTimer::singleShot(0, this, [this]() {
                string newText = ui->plainTextEdit->toPlainText().toStdString();

                if (newText != currentText) {
                    saveSnapshot(currentText, undoStack, redoStack,
                                 historyList, historyQueue, newText);
                }
            });
        }
    }
    return QWidget::eventFilter(watched, event);
}

void EditorWindow::handleUndoButton()
{
    string result = undoAction(currentText, undoStack, redoStack);
    refreshTextEdit(result);
}

void EditorWindow::handleRedoButton()
{
    string result = redoAction(currentText, undoStack, redoStack);
    refreshTextEdit(result);
}

void EditorWindow::refreshTextEdit(const string &text)
{
    ui->plainTextEdit->blockSignals(true);
    ui->plainTextEdit->setPlainText(QString::fromStdString(text));
    ui->plainTextEdit->blockSignals(false);
}

bool EditorWindow::hasUnsavedChanges() const
{
    return currentText != savedText;
}

// ============================================================
//  SAVE
// ============================================================
void EditorWindow::handleSaveButton()
{
    pendingCloseAfterSave = false;
    performSave();
}

void EditorWindow::performSave()
{
    if (filePath.empty()) {
        // FITUR 1: file baru -> minta nama dulu lewat popup
        dialogs *dlg = new dialogs(this);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->setSaveLocationText(QString::fromStdString(loadConfig()));

        connect(dlg, &dialogs::saveConfirmed, this, &EditorWindow::onSaveAsConfirmed);
        connect(dlg, &dialogs::cancelled, dlg, &QWidget::close);

        dlg->showPage(dialogs::PageSaveNew);
    } else {
        // FITUR 2: file lama -> langsung overwrite ke path yang sudah ada
        writeToFile(QString::fromStdString(filePath));
    }
}

void EditorWindow::onSaveAsConfirmed(const QString &fileName)
{
    string savePath = loadConfig();
    ensureSaveDir(savePath);

    string name = fileName.toStdString();
    if (name.size() < 4 || name.substr(name.size() - 4) != ".txt") {
        name += ".txt";
    }

    string fullPath = savePath + name;
    writeToFile(QString::fromStdString(fullPath));
    catatKeIndex(savePath, name);

    if (auto *dlg = qobject_cast<dialogs*>(sender())) {
        dlg->close();
    }
}

void EditorWindow::writeToFile(const QString &path)
{
    bool ok = saveFile(path.toStdString(), currentText);
    if (!ok) return; // bisa ditambah popup error kalau perlu nanti

    filePath  = path.toStdString();
    savedText = currentText;

    QFileInfo info(path);
    ui->labelNamaFile->setText("File: " + info.fileName());

    dialogs *successDlg = new dialogs(this);
    successDlg->setAttribute(Qt::WA_DeleteOnClose);
    connect(successDlg, &dialogs::okClicked, this, &EditorWindow::onSaveSuccessAck);
    successDlg->showPage(dialogs::PageSuccess);
}

void EditorWindow::onSaveSuccessAck()
{
    if (pendingCloseAfterSave) {
        pendingCloseAfterSave = false;
        closeEditor();
    }
}

// ============================================================
//  FITUR 3: PERINGATAN BELUM DISIMPAN
// ============================================================
void EditorWindow::handleBackButton()
{
    if (hasUnsavedChanges()) {
        dialogs *dlg = new dialogs(this);
        dlg->setAttribute(Qt::WA_DeleteOnClose);

        connect(dlg, &dialogs::saveBeforeExit, this, &EditorWindow::onExitWithSave);
        connect(dlg, &dialogs::discardChanges, this, &EditorWindow::onExitWithoutSave);
        connect(dlg, &dialogs::cancelled, dlg, &QWidget::close);

        dlg->showPage(dialogs::PageAlertNotSaved);
        return; // tunggu pilihan user, jangan close dulu
    }
    closeEditor();
}

void EditorWindow::onExitWithSave()
{
    pendingCloseAfterSave = true;
    performSave();

    if (auto *dlg = qobject_cast<dialogs*>(sender())) {
        dlg->close();
    }
}

void EditorWindow::onExitWithoutSave()
{
    if (auto *dlg = qobject_cast<dialogs*>(sender())) {
        dlg->close();
    }
    closeEditor();
}

void EditorWindow::closeEditor()
{
    if (mainWindowRef) {
        mainWindowRef->show();
    }
    this->close();
}

void EditorWindow::toggleHistoryDropdown()
{
    if (ui->historyListWidget->isVisible()) {
        ui->historyListWidget->hide();
        return;
    }

    ui->historyListWidget->clear();

    DLLNode *node = historyList.head;
    while (node != nullptr) {
        QString fullText = QString::fromStdString(node->text).simplified();

        QString lastWord;
        if (fullText.isEmpty()) {
            lastWord = "(kosong)";
        } else {
            QStringList words = fullText.split(' ', Qt::SkipEmptyParts);
            lastWord = words.isEmpty() ? "(kosong)" : words.last();
        }

        QListWidgetItem *item = new QListWidgetItem(lastWord);
        item->setData(Qt::UserRole, QVariant::fromValue(reinterpret_cast<qulonglong>(node)));
        ui->historyListWidget->addItem(item);
        node = node->next;
    }

    ui->historyListWidget->show();
    ui->historyListWidget->raise();
}

void EditorWindow::onHistoryItemClicked(QListWidgetItem *item)
{
    DLLNode *target = reinterpret_cast<DLLNode*>(item->data(Qt::UserRole).toULongLong());
    string result = jumpToSnapshot(currentText, undoStack, redoStack, historyList, target);
    refreshTextEdit(result);
    ui->historyListWidget->hide();
}