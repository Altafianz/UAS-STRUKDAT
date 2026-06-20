#include "dialogs.h"
#include "ui_dialogs.h"

dialogs::dialogs(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::dialogs)
{
    ui->setupUi(this);
}

dialogs::~dialogs()
{
    delete ui;
}
