#ifndef DIALOGS_H
#define DIALOGS_H

#include <QWidget>

namespace Ui {
class dialogs;
}

class dialogs : public QWidget
{
    Q_OBJECT

public:
    explicit dialogs(QWidget *parent = nullptr);
    ~dialogs();

    enum Page {
        PageSaveNew       = 0,  // popup_save
        PageAlertNotSaved = 1,  // popup_alert_notSave
        PageSuccess       = 2   // page_4
    };

    void showPage(Page page);
    void setSaveLocationText(const QString &text);

signals:
    void saveConfirmed(const QString &fileName);  // dari popup_save
    void cancelled();                             // dari cancel manapun
    void saveBeforeExit();                        // dari popup_alert_notSave -> "Save"
    void discardChanges();                        // dari popup_alert_notSave -> "Don't Save"
    void okClicked();                             // dari page_4 -> "OK"

private slots:
    void onSaveButtonPopupClicked();
    void onCancelButtonPopupClicked();
    void onAlertSaveClicked();
    void onAlertDontSaveClicked();
    void onAlertCancelClicked();
    void onSuccessOkClicked();

private:
    Ui::dialogs *ui;
};

#endif // DIALOGS_H