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

// konstruktor: setup tabel daftar file, hubungkan tombol & klik baris, lalu langsung muat data
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

    // menghubungkan sinyal dari qt ke logika
    connect(ui->cancelButton, &QPushButton::clicked, this, &OpenFileWindow::handleBackButtonClicked);
    connect(ui->openButton, &QPushButton::clicked, this, &OpenFileWindow::handleOpenButtonClicked);
    connect(ui->fileTableWidget, &QTableWidget::itemClicked, this, &OpenFileWindow::onFileRowClicked);
    connect(ui->fileTableWidget, &QTableWidget::itemDoubleClicked, this, &OpenFileWindow::handleOpenButtonClicked);

    // ambil daftar file dari index.txt, lalu tampilkan langsung ke tabel
    loadFileEntries();
    renderFileTable();
}

OpenFileWindow::~OpenFileWindow()
{
    delete ui;
}

// mengambil daftar nama file dari index.txt (lewat loadIndex di filemanager), lalu disusun jadi fileEntries
void OpenFileWindow::loadFileEntries()
{
    fileEntries.clear();

    string savePath = loadConfig();
    int count = 0;
    string *fileNames = loadIndex(savePath, count);

    if (fileNames == nullptr || count == 0)
    {
        return;
    }

    for (int i = 0; i < count; i++)
    {
        QString name = QString::fromStdString(fileNames[i]);
        QString fullPath = QString::fromStdString(savePath) + name;
        QFileInfo info(fullPath);

        FileEntry entry;
        entry.name = name;
        entry.fullPath = fullPath;
        entry.modified = info.exists() ? info.lastModified() : QDateTime();

        fileEntries.append(entry);
    }

    delete[] fileNames;
}

// menampilkan ulang isi fileEntries ke fileTableWidget (dipanggil tiap kali data berubah, misal habis sorting)
void OpenFileWindow::renderFileTable()
{
    ui->fileTableWidget->setRowCount(fileEntries.size());

    for (int i = 0; i < fileEntries.size(); i++)
    {
        const FileEntry &entry = fileEntries[i];

        QTableWidgetItem *noItem = new QTableWidgetItem(QString::number(i + 1));
        noItem->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *nameItem = new QTableWidgetItem(entry.name);
        nameItem->setData(Qt::UserRole, entry.fullPath);

        QString dateText = entry.modified.isValid()
                               ? entry.modified.toString("dd/MM/yy")
                               : "-";
        QTableWidgetItem *dateItem = new QTableWidgetItem(dateText);

        ui->fileTableWidget->setItem(i, 0, noItem);
        ui->fileTableWidget->setItem(i, 1, nameItem);
        ui->fileTableWidget->setItem(i, 2, dateItem);
    }
}

// mengurutkan fileEntries berdasarkan nama file (A-Z / Z-A) memakai bubble sort
void OpenFileWindow::bubbleSortByName(bool ascending)
{
    int n = fileEntries.size();

    for (int i = 0; i < n - 1; i++)
    {
        bool swapped = false;

        for (int j = 0; j < n - 1 - i; j++)
        {
            QString a = fileEntries[j].name.toLower();
            QString b = fileEntries[j + 1].name.toLower();

            bool needSwap = ascending ? (a > b) : (a < b);

            if (needSwap)
            {
                FileEntry temp = fileEntries[j];
                fileEntries[j] = fileEntries[j + 1];
                fileEntries[j + 1] = temp;
                swapped = true;
            }
        }

        if (!swapped)
            break; // udah terurut, berhenti lebih awal
    }
}

// mengurutkan fileEntries berdasarkan tanggal modifikasi (lama-baru / baru-lama) memakai bubble sort
void OpenFileWindow::bubbleSortByDate(bool ascending)
{
    int n = fileEntries.size();

    for (int i = 0; i < n - 1; i++)
    {
        bool swapped = false;

        for (int j = 0; j < n - 1 - i; j++)
        {
            QDateTime a = fileEntries[j].modified;
            QDateTime b = fileEntries[j + 1].modified;

            bool needSwap = ascending ? (a > b) : (a < b);

            if (needSwap)
            {
                FileEntry temp = fileEntries[j];
                fileEntries[j] = fileEntries[j + 1];
                fileEntries[j + 1] = temp;
                swapped = true;
            }
        }

        if (!swapped)
            break;
    }
}

// menghubungkan ui untuk melakukan sorting nama file, lalu render ulang tabel
void OpenFileWindow::handleSortByNameClicked()
{
    bubbleSortByName(sortNameAscending);
    sortNameAscending = !sortNameAscending;
    renderFileTable();
}

// menghubungkan ui untuk melakukan sorting tanggal diubah, lalu render ulang tabel
void OpenFileWindow::handleSortByDateClicked()
{
    bubbleSortByDate(sortDateAscending);
    sortDateAscending = !sortDateAscending;
    renderFileTable();
}

// dipakai untuk menyimpan path file yang sedang dipilih user di tabel, supaya dipakai saat tombol Open diklik
void OpenFileWindow::onFileRowClicked(QTableWidgetItem *item)
{
    QTableWidgetItem *nameItem = ui->fileTableWidget->item(item->row(), 1);
    if (nameItem)
    {
        selectedFilePath = nameItem->data(Qt::UserRole).toString();
    }
}

// fungsi saat kembali ke home dari halaman open file
void OpenFileWindow::handleBackButtonClicked()
{
    if (mainWindowRef)
    {
        mainWindowRef->show();
    }
    this->close();
}

// dipakai untuk membuka EditorWindow dengan file yang sudah dipilih dari tabel
void OpenFileWindow::handleOpenButtonClicked()
{
    // peringatan kalau user klik Open tanpa pilih file dari daftar dulu
    if (selectedFilePath.isEmpty())
    {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("Belum ada file");
        msgBox.setText("Silakan pilih file dari daftar terlebih dahulu.");

        msgBox.setStyleSheet(
            "QLabel { font-family: 'Inter'; font-weight: 500; font-size: 14px; color: #1e293b; }"
            "QPushButton { font-family: 'Inter'; font-size: 13px; min-width: 80px; color: #1e293b; }");

        msgBox.exec();
        return;
    }

    EditorWindow *editor = new EditorWindow(mainWindowRef, selectedFilePath);
    editor->show();
    this->close();
}