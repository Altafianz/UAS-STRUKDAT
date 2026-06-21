#include "openfilewindow.h"
#include "ui_openfilewindow.h"
#include "mainwindow.h"
#include "editorwindow.h"
#include "../core/filemanager.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QDateTime>
#include <QHeaderView>

OpenFileWindow::OpenFileWindow(MainWindow *mainWin, QWidget *parent)
    : QWidget(parent), ui(new Ui::OpenFileWindow), mainWindowRef(mainWin)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    ui->fileTableWidget->setColumnCount(3);

    QStringList headers = {"No", "Nama File", "Tanggal Diubah"};
    ui->fileTableWidget->setHorizontalHeaderLabels(headers);

    ui->fileTableWidget->verticalHeader()->setVisible(false); // matiin nomor bawaan, dobel sama kolom "No"

    ui->fileTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->fileTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->fileTableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    ui->fileTableWidget->setFrameShape(QFrame::NoFrame);
    ui->fileTableWidget->setShowGrid(true);
    ui->fileTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->fileTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->fileTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(ui->cancelButton, &QPushButton::clicked, this, &OpenFileWindow::handleBackButtonClicked);
    connect(ui->openButton, &QPushButton::clicked, this, &OpenFileWindow::handleOpenButtonClicked);
    connect(ui->fileTableWidget, &QTableWidget::itemClicked, this, &OpenFileWindow::onFileRowClicked);
    connect(ui->fileTableWidget, &QTableWidget::itemDoubleClicked, this, &OpenFileWindow::handleOpenButtonClicked);

    populateFileTable();
}

OpenFileWindow::~OpenFileWindow()
{
    delete ui;
}

void OpenFileWindow::populateFileTable()
{
    ui->fileTableWidget->setRowCount(0);

    string savePath = loadConfig();
    int count = 0;
    string* fileNames = loadIndex(savePath, count);

    if (fileNames == nullptr || count == 0) {
        return;
    }

    ui->fileTableWidget->setRowCount(count);

    for (int i = 0; i < count; i++) {
        QString name     = QString::fromStdString(fileNames[i]);
        QString fullPath = QString::fromStdString(savePath) + name;

        QFileInfo info(fullPath);
        QString modifiedDate = info.exists()
                                    ? info.lastModified().toString("dd/MM/yy")
                                    : "-";

        QTableWidgetItem *noItem = new QTableWidgetItem(QString::number(i + 1));
        noItem->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *nameItem = new QTableWidgetItem(name);
        nameItem->setData(Qt::UserRole, fullPath); // full path tetap nempel di kolom nama

        QTableWidgetItem *dateItem = new QTableWidgetItem(modifiedDate);

        ui->fileTableWidget->setItem(i, 0, noItem);
        ui->fileTableWidget->setItem(i, 1, nameItem);
        ui->fileTableWidget->setItem(i, 2, dateItem);
    }

    delete[] fileNames;
}

void OpenFileWindow::onFileRowClicked(QTableWidgetItem *item)
{
    QTableWidgetItem *nameItem = ui->fileTableWidget->item(item->row(), 1);
    if (nameItem) {
        selectedFilePath = nameItem->data(Qt::UserRole).toString();
    }
}

void OpenFileWindow::handleBackButtonClicked()
{
    if (mainWindowRef) {
        mainWindowRef->show();
    }
    this->close();
}

void OpenFileWindow::handleOpenButtonClicked()
{
    if (selectedFilePath.isEmpty()) {
        QMessageBox::warning(this, "Belum ada file", "Silakan pilih file dari daftar terlebih dahulu.");
        return;
    }

    EditorWindow *editor = new EditorWindow(mainWindowRef, selectedFilePath);
    editor->show();
    this->close();
}