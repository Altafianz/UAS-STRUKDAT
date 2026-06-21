#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "editorwindow.h"
#include "openfilewindow.h"
#include "creditwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->newFileButton, &QPushButton::clicked, this, &MainWindow::on_newFile_clicked);
    connect(ui->openFileButton, &QPushButton::clicked, this, &MainWindow::on_openFile_clicked);
    connect(ui->aboutButton, &QPushButton::clicked, this, &MainWindow::on_about_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_newFile_clicked()
{
    EditorWindow *editor = new EditorWindow(this);
    editor->show();
    this->hide();
}

void MainWindow::on_openFile_clicked()
{
    OpenFileWindow *openFile = new OpenFileWindow(this);
    openFile->show();
    this->hide();
}

void MainWindow::on_about_clicked()
{
    CreditWindow *credit = new CreditWindow(this);
    credit->show();
    this->hide();
}
