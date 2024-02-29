#ifndef SAVEDWORDS_H
#define SAVEDWORDS_H

#include <QWidget>
#include <QFile>
#include <QTextStream>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

namespace Ui {
class SavedWordsAppWidget;
}

class SavedWordsAppWidget : public QWidget
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit SavedWordsAppWidget(QWidget *parent = nullptr);
    ~SavedWordsAppWidget();
    QString words;
    void checkwords();
    void save_word_init();

private slots:
    void on_backBtn_clicked();
    void on_clearBtn_clicked();

private:
    Ui::SavedWordsAppWidget *ui;
};

#endif // SAVEDWORDS_H
