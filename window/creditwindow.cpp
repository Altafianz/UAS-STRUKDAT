#include "creditwindow.h"
#include "./ui_creditwindow.h"
#include "mainwindow.h"

CreditWindow::CreditWindow(MainWindow *mainWin, QWidget *parent)
    : QWidget(parent), ui(new Ui::CreditWindow), mainWindowRef(mainWin)
{
    ui->setupUi(this);

    connect(ui->backButtonCredit, &QPushButton::clicked, this, &CreditWindow::on_backButton_clicked);
}

CreditWindow::~CreditWindow()
{
    delete ui;
}

void CreditWindow::on_backButton_clicked()
{
    if (mainWindowRef) {
        mainWindowRef->show();
    }
    this->close();
}