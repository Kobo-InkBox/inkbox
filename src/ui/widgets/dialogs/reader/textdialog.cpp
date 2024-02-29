#include "textdialog.h"
#include "ui_textdialog.h"

TextDialog::TextDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextDialog)
{
    ui->setupUi(this);
    ui->highlightBtn->setProperty("type", "borderless");
    if(global::reader::highlightAlreadyDone == true) {
        global::reader::highlightAlreadyDone = false;
        highlightAlreadyDone = true;
        ui->highlightBtn->setIcon(QIcon(":/resources/unhighlight.png"));
    }
    else {
        highlightAlreadyDone = false;
        ui->highlightBtn->setIcon(QIcon(":/resources/highlight.png"));
    }
    this->adjustSize();
}

TextDialog::~TextDialog()
{
    delete ui;
}

void TextDialog::on_highlightBtn_clicked()
{
    if(highlightAlreadyDone == true) {
        emit unhighlightText();
    }
    else {
        emit highlightText();
    }
    TextDialog::close();
}
