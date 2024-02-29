#ifndef SEARCHRESULTSWIDGET_H
#define SEARCHRESULTSWIDGET_H

#include <QWidget>
#include <QStringListModel>
#include "bookinfodialog.h"

namespace Ui {
class searchResultsWidget;
}

class searchResultsWidget : public QWidget
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit searchResultsWidget(QWidget *parent = nullptr);
    ~searchResultsWidget();
    void setListViewContents(QStringList searchResults);
    QModelIndex index;
    QString item;
    bool libraryResults;

private slots:
    void on_openBtn_clicked();
    void on_backBtn_clicked();
    void showToastNative(QString messageToDisplay);
    void closeIndefiniteToastNative();

private:
    Ui::searchResultsWidget *ui;
    BookInfoDialog * bookInfoDialogWindow;

signals:
    void openBookFile(QString book, bool relativePath);
    void showToast(QString messageToDisplay);
    void closeIndefiniteToast();
    void hideDialog();
};

#endif // SEARCHRESULTSWIDGET_H
