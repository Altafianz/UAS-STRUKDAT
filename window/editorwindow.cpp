#include "editorwindow.h"
#include "ui_editorwindow.h"
#include "mainwindow.h"
#include <QKeyEvent>
#include <QTimer>

EditorWindow::EditorWindow(MainWindow *mainWin, QWidget *parent)
    : QWidget(parent), ui(new Ui::EditorWindow), mainWindowRef(mainWin)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    initStack(undoStack);
    initStack(redoStack);
    initHistoryList(historyList);
    initQueue(historyQueue);
    currentText = "";
    appendHistory(historyList, currentText);

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

void EditorWindow::handleBackButton()
{
    if (mainWindowRef) {
        mainWindowRef->show();
    }
    this->close();
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

void EditorWindow::handleSaveButton()
{
    if (filePath.empty()) {
        filePath = "./saves/Unnamed.txt";
    }
    saveFile(filePath, currentText);
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

        QString label = QString("%1").arg(lastWord);

        QListWidgetItem *item = new QListWidgetItem(label);
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