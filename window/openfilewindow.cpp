#include "openfilewindow.h"
#include "ui_openfilewindow.h"

OpenFileWindow::OpenFileWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OpenFileWindow)
{
    ui->setupUi(this);
}

OpenFileWindow::~OpenFileWindow()
{
    delete ui;
}
