#ifndef DICTIONARYWIDGET_H
#define DICTIONARYWIDGET_H

#include "functions.h"

#include <QWidget>

namespace Ui {
class dictionaryWidget;
}

class dictionaryWidget : public QWidget
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit dictionaryWidget(QWidget *parent = nullptr);
    ~dictionaryWidget();
    void dictionaryLookup(std::string word, QString first_letter, int position);
    QString wordQstr;
    QString definition;
    QString letter;
    std::string searchedWord;
    bool nextdefinition_lock = false;
    int dictionaryPosition;

private slots:
    void on_backBtn_clicked();
    void on_nextDefinitionBtn_clicked();
    void on_previousDefinitionBtn_clicked();
    void refreshScreenNative();

private:
    Ui::dictionaryWidget *ui;

signals:
    void refreshScreen();
};

#endif // DICTIONARYWIDGET_H
