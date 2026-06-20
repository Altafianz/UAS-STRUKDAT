#include "dialogs.h"
#include "ui_dialogs.h"

dialogs::dialogs(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::dialogs)
{
    ui->setupUi(this);

    // tombol save abu-abu nyala kalau nama file masih kosong
    ui->SaveButtonPopup->setVisible(false);
    ui->SaveButtonPopup_grey->setVisible(true);

    connect(ui->lineEdit, &QLineEdit::textChanged, this, [this](const QString &text) {
        bool empty = text.trimmed().isEmpty();
        ui->SaveButtonPopup->setVisible(!empty);
        ui->SaveButtonPopup_grey->setVisible(empty);
    });

    connect(ui->SaveButtonPopup,   &QPushButton::clicked, this, &dialogs::onSaveButtonPopupClicked);
    connect(ui->cancelButtonPopup, &QPushButton::clicked, this, &dialogs::onCancelButtonPopupClicked);

    connect(ui->pushButton_2, &QPushButton::clicked, this, &dialogs::onAlertSaveClicked);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &dialogs::onAlertDontSaveClicked);
    connect(ui->pushButton_4, &QPushButton::clicked, this, &dialogs::onAlertCancelClicked);

    connect(ui->pushButton, &QPushButton::clicked, this, &dialogs::onSuccessOkClicked);
}

dialogs::~dialogs()
{
    delete ui;
}

void dialogs::showPage(Page page)
{
    ui->stackedWidget->setCurrentIndex(static_cast<int>(page));

    // posisikan di tengah-tengah parent (EditorWindow)
    if (QWidget *parentW = parentWidget()) {
        int x = (parentW->width()  - width())  / 2;
        int y = (parentW->height() - height()) / 2;
        move(x, y);
    }

    show();
    raise();
}

void dialogs::setSaveLocationText(const QString &text)
{
    ui->lineEdit_2->setText(text);
}

void dialogs::onSaveButtonPopupClicked()
{
    QString name = ui->lineEdit->text().trimmed();
    if (name.isEmpty()) return;
    emit saveConfirmed(name);
}

void dialogs::onCancelButtonPopupClicked() { emit cancelled(); }
void dialogs::onAlertSaveClicked()         { emit saveBeforeExit(); }
void dialogs::onAlertDontSaveClicked()     { emit discardChanges(); }
void dialogs::onAlertCancelClicked()       { emit cancelled(); }

void dialogs::onSuccessOkClicked()
{
    emit okClicked();
    this->close();
}