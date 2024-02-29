#ifndef BOOKOPTIONSDIALOG_H
#define BOOKOPTIONSDIALOG_H

#include <QDialog>
#include <QJsonObject>

namespace Ui {
class BookOptionsDialog;
}

class BookOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit BookOptionsDialog(QWidget *parent = nullptr);
    ~BookOptionsDialog();
    QString bookPath;
    QString bookChecksum;
    bool bookPinned;
    bool isFolder;

private slots:
    void on_pinBtn_clicked();
    void on_deleteBtn_clicked();
    void on_infoBtn_clicked();
    void on_wipeLocalReadingSettingsBtn_clicked();
    void pinBook(int bookID);
    void unpinBook(int bookID);
    void deleteBook();
    void deleteFolder();
    bool isBookPinned(int bookID);

signals:
    void openLocalBookInfoDialog();
    void showToast(QString messageToDisplay);
    void removedFolder();

private:
    Ui::BookOptionsDialog *ui;
};

#endif // BOOKOPTIONSDIALOG_H
