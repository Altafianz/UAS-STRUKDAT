#include "openfilewindow.h"
#include "ui_openfilewindow.h"
#include "mainwindow.h"

OpenFileWindow::OpenFileWindow(MainWindow *mainWin, QWidget *parent)
    : QWidget(parent), ui(new Ui::OpenFileWindow), mainWindowRef(mainWin)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    connect(ui->cancelButton, &QPushButton::clicked, this, &OpenFileWindow::on_backButton_clicked);
}

OpenFileWindow::~OpenFileWindow()
{
    delete ui;
}

void OpenFileWindow::on_backButton_clicked()
{
    if (mainWindowRef) {
        mainWindowRef->show();
    }
    this->close();
}