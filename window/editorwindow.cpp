#include "editorwindow.h"
#include "ui_editorwindow.h"
#include "mainwindow.h"
#include <QColor>

EditorWindow::EditorWindow(MainWindow *mainWin, QWidget *parent)
    : QWidget(parent), ui(new Ui::EditorWindow), mainWindowRef(mainWin)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose); // biar otomatis ke-delete pas ditutup

    connect(ui->backButton, &QPushButton::clicked, this, &EditorWindow::on_backButton_clicked);
}

EditorWindow::~EditorWindow()
{
    delete ui;
}

void EditorWindow::on_backButton_clicked()
{
    if (mainWindowRef) {
        mainWindowRef->show(); 
    }
    this->close();
}