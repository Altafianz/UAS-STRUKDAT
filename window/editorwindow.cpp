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

    // inisialisasi struct data (stack dan history list undo)
    initStack(undoStack);
    initStack(redoStack);
    initHistoryList(historyList);
    initQueue(historyQueue);

    // cabang logika, jika dibuka dari dari openFile, isinya sesuai yang sudah ada. jika baru, isinya kosong
    if (!existingFilePath.isEmpty()) {
        filePath    = existingFilePath.toStdString();
        currentText = loadFile(filePath);

        QFileInfo info(existingFilePath);
        ui->labelNamaFile->setText("File: " + info.fileName());
    } else {
        currentText = "";
    }

    // baseline buat deteksi perubahan
    savedText = currentText; 
    appendHistory(historyList, currentText);
    refreshTextEdit(currentText);

    // untuk fungsi eventFiltering
    ui->plainTextEdit->installEventFilter(this);
    ui->historyListWidget->hide();

    // menghubungkan sinyal dari qt ke logika 
    connect(ui->backButton, &QPushButton::clicked, this, &EditorWindow::handleBackButton);
    connect(ui->undoButton, &QPushButton::clicked, this, &EditorWindow::handleUndoButton);
    connect(ui->redoButton, &QPushButton::clicked, this, &EditorWindow::handleRedoButton);
    connect(ui->saveButton, &QPushButton::clicked, this, &EditorWindow::handleSaveButton);
    connect(ui->dropDown,   &QPushButton::clicked, this, &EditorWindow::toggleHistoryDropdown);
    connect(ui->historyListWidget, &QListWidget::itemClicked,this, &EditorWindow::onHistoryItemClicked);
}

EditorWindow::~EditorWindow()
{
    delete ui;
}

// fungsi push otomatis ke stck undo apabila user menekan spasi atau enter
bool EditorWindow::eventFilter(QObject *watched, QEvent *event)
{
    // memastikan event yang diperhatikan hanya di plainTextEdit
    if (watched == ui->plainTextEdit && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        // logika jika user menekan tombol spasi atau enter
        if (keyEvent->key() == Qt::Key_Space || keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {

            QTimer::singleShot(0, this, [this]() {
                string newText = ui->plainTextEdit->toPlainText().toStdString();
                if (newText != currentText) {
                    saveSnapshot(currentText, undoStack, redoStack, historyList, historyQueue, newText);
                }
            });
        }
    }
    return QWidget::eventFilter(watched, event);
}

// memuat ulang teks baru setelah melakukan undo/redo
void EditorWindow::refreshTextEdit(const string &text)
{
    ui->plainTextEdit->blockSignals(true);
    ui->plainTextEdit->setPlainText(QString::fromStdString(text));
    ui->plainTextEdit->blockSignals(false);
}

// menghubungkan ui untuk melakukan aksi undo
void EditorWindow::handleUndoButton()
{
    string result = undoAction(currentText, undoStack, redoStack);
    refreshTextEdit(result);
}

// menghubungkan ui untuk melakukan aksi redo
void EditorWindow::handleRedoButton()
{
    string result = redoAction(currentText, undoStack, redoStack);
    refreshTextEdit(result);
}

// dipakai untuk logika saat user menekan back namun belum save
bool EditorWindow::hasUnsavedChanges() const
{
    return currentText != savedText;
}

// save secara manual (bukan saat user menekan back)
void EditorWindow::handleSaveButton()
{
    pendingCloseAfterSave = false;
    performSave();
}

// memunvulkan teks box pada saat save
void EditorWindow::performSave()
{
    if (filePath.empty()) {
        // memunculkan dialog save file untuk file baru
        dialogs *dlg = new dialogs(this);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->setSaveLocationText(QString::fromStdString(loadConfig()));

        connect(dlg, &dialogs::saveConfirmed, this, &EditorWindow::onSaveAsConfirmed);
        connect(dlg, &dialogs::cancelled, dlg, &QWidget::close);

        dlg->showPage(dialogs::PageSaveNew);
    } else {
        // file sudah ada, langsung auto save
        writeToFile(QString::fromStdString(filePath));
    }
}

// logika proses saat melakukan save
void EditorWindow::onSaveAsConfirmed(const QString &fileName)
{
    string savePath = loadConfig();
    ensureSaveDir(savePath);

    // mengecek kalau user menambahkan tulisan .txt manual
    string name = fileName.toStdString();
    if (name.size() < 4 || name.substr(name.size() - 4) != ".txt") {
        name += ".txt";
    }

    // menyimpan file ke penyimpanan local
    string fullPath = savePath + name;
    writeToFile(QString::fromStdString(fullPath));

    // mencatat ke indeks untuk ditampilkan di halaman open file
    catatKeIndex(savePath, name);

    if (auto *dlg = qobject_cast<dialogs*>(sender())) {
        dlg->close();
    }
}

// proses menyimpan (yang dihubungkan dari performSave)
void EditorWindow::writeToFile(const QString &path)
{
    bool ok = saveFile(path.toStdString(), currentText);
    if (!ok) return;
    
    // meyimpan teks terakhir untuk dibandingkan pada saat save file di pengeditan selanjutnya
    filePath = path.toStdString();
    savedText = currentText;

    // mengambil nama filenya saja dari seleuruh path untuk ditampilkan di halaman openFile
    QFileInfo info(path);
    ui->labelNamaFile->setText("File: " + info.fileName());

    // memunculkan popup save berhasil 
    dialogs *successDlg = new dialogs(this);
    successDlg->setAttribute(Qt::WA_DeleteOnClose);
    connect(successDlg, &dialogs::okClicked, this, &EditorWindow::onSaveSuccessAck);
    successDlg->showPage(dialogs::PageSuccess);
}

// kalo user save melalui tombol back
void EditorWindow::onSaveSuccessAck()
{
    if (pendingCloseAfterSave) {
        pendingCloseAfterSave = false;
        closeEditor();
    }
}

//  FITUR PERINGATAN BELUM DISIMPAN
void EditorWindow::handleBackButton()
{
    // dari fungsi sebelumnya, jika tru (isi file berubah, user langsung menekan back, belum save)
    if (hasUnsavedChanges()) {
        // muncul popup peringatan
        dialogs *dlg = new dialogs(this);
        dlg->setAttribute(Qt::WA_DeleteOnClose);

        connect(dlg, &dialogs::saveBeforeExit, this, &EditorWindow::onExitWithSave);
        connect(dlg, &dialogs::discardChanges, this, &EditorWindow::onExitWithoutSave);
        connect(dlg, &dialogs::cancelled, dlg, &QWidget::close);

        dlg->showPage(dialogs::PageAlertNotSaved);
        return; 
    }
    closeEditor();
}

// lanjutan saat muncul popup peringatan

//jika user menekan save 
void EditorWindow::onExitWithSave()
{
    pendingCloseAfterSave = true;
    performSave();

    if (auto *dlg = qobject_cast<dialogs*>(sender())) {
        dlg->close();
    }
}

// jika user menekan cancel
void EditorWindow::onExitWithoutSave()
{
    if (auto *dlg = qobject_cast<dialogs*>(sender())) {
        dlg->close();
    }
    closeEditor();
}

// fungsi saay kembali ke home dari halaman editor
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