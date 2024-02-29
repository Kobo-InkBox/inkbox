#include "dictionarywidget.h"
#include "ui_dictionarywidget.h"

#include <QDir>
#include <QTimer>
#include <QProcess>
#include <QTextStream>
#include <QDebug>
#include <iostream>
#include <QDebug>

dictionaryWidget::dictionaryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::dictionaryWidget)
{
    ui->setupUi(this);
    ui->wordLabel->setStyleSheet("font-weight: bold");
    ui->definitionStatusLabel->setFont(QFont("u001"));

    ui->backBtn->setProperty("type", "borderless");
    ui->backBtn->setStyleSheet("background: lightGrey; font-size: 9pt; padding: 8px");
    ui->nextDefinitionBtn->setProperty("type", "borderless");
    ui->previousDefinitionBtn->setProperty("type", "borderless");

    ui->nextDefinitionBtn->setText("");
    ui->nextDefinitionBtn->setIcon(QIcon(":/resources/chevron-right.png"));
    ui->previousDefinitionBtn->setText("");
    ui->previousDefinitionBtn->setIcon(QIcon(":/resources/chevron-left.png"));

    QStringList parts = global::keyboard::keyboardText.split(' ', QString::SkipEmptyParts);
    for (int i = 0; i < parts.size(); ++i)
        parts[i].replace(0, 1, parts[i][0].toUpper());
    wordQstr = parts.join(" ");
    searchedWord = wordQstr.toStdString();
    letter = global::keyboard::keyboardText.left(1);
    letter = letter.toUpper();
    dictionaryPosition = 1;

    dictionaryLookup(searchedWord, letter, dictionaryPosition);

    definition.prepend("<div align='justify'>");
    definition.append("</div>");
    ui->wordLabel->setText(wordQstr);
    ui->definitionLabel->setText(definition);
    ui->definitionStatusLabel->setText("1");

    QTimer::singleShot(1000, this, SLOT(refreshScreenNative()));
}

dictionaryWidget::~dictionaryWidget()
{
    delete ui;
}

void dictionaryWidget::on_backBtn_clicked()
{
    dictionaryWidget::close();
}

void dictionaryWidget::dictionaryLookup(std::string word, QString first_letter, int position) {
    log("Dictionary lookup requested for word '" + QString::fromStdString(word) + "', position " + QString::number(position), className);
    QDir dictdir;
    dictdir.mkpath("/inkbox/dictionary");

    std::ofstream fhandler;
    fhandler.open("/inkbox/dictionary/word");
    fhandler << word;
    fhandler.close();

    QDir::setCurrent("dictionary");
    QDir::setCurrent(first_letter);
    QString lookup_prog ("sh");
    QStringList lookup_args;
    QString position_str = QString::number(position);
    lookup_args << "../scripts/lookup.sh" << position_str;
    QProcess *lookup_proc = new QProcess();
    lookup_proc->start(lookup_prog, lookup_args);
    lookup_proc->waitForFinished();
    lookup_proc->deleteLater();

    QFile definition_file("/inkbox/dictionary/definition");
    definition_file.open(QIODevice::ReadWrite);
    QTextStream in (&definition_file);
    definition = in.readAll();
    definition = definition.remove(QRegExp("[\n]"));
    if(definition == "No definition found.") {
        nextdefinition_lock = true;
    }
    else {
        nextdefinition_lock = false;
    }
    definition_file.close();

    setDefaultWorkDir();
}

void dictionaryWidget::on_nextDefinitionBtn_clicked()
{
    dictionaryPosition = dictionaryPosition + 1;
    dictionaryLookup(searchedWord, letter, dictionaryPosition);
    if(nextdefinition_lock == true) {
        dictionaryPosition = dictionaryPosition - 1;
    }
    else {
        ui->definitionLabel->setText(definition);
        QString dictionaryPositionQstr = QString::number(dictionaryPosition);
        ui->definitionStatusLabel->setText(dictionaryPositionQstr);
    }
}

void dictionaryWidget::on_previousDefinitionBtn_clicked()
{
    dictionaryPosition = dictionaryPosition - 1;
    if(dictionaryPosition <= 0) {
        dictionaryPosition = 1;
    }
    else {
        dictionaryLookup(searchedWord, letter, dictionaryPosition);
        ui->definitionLabel->setText(definition);
        QString dictionaryPositionQstr = QString::number(dictionaryPosition);
        ui->definitionStatusLabel->setText(dictionaryPositionQstr);
    }
}

void dictionaryWidget::refreshScreenNative() {
    emit refreshScreen();
}
