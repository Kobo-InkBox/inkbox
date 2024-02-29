#include "virtualkeypad.h"
#include "ui_virtualkeypad.h"
#include "reader.h"
#include <QDebug>

virtualkeypad::virtualkeypad(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::virtualkeypad)
{
    ui->setupUi(this);
    ui->lineEdit->setFont(QFont("u001"));

    // Style
    ui->n1->setProperty("type", "borderless");
    ui->n2->setProperty("type", "borderless");
    ui->n3->setProperty("type", "borderless");
    ui->n4->setProperty("type", "borderless");
    ui->n5->setProperty("type", "borderless");
    ui->n6->setProperty("type", "borderless");
    ui->n7->setProperty("type", "borderless");
    ui->n8->setProperty("type", "borderless");
    ui->n9->setProperty("type", "borderless");
    ui->n0->setProperty("type", "borderless");
    ui->eraseBtn->setProperty("type", "borderless");
    ui->clearBtn->setProperty("type", "borderless");

    ui->n1->setStyleSheet("font-weight: bold; font-size: 13pt");
    ui->n2->setStyleSheet("font-weight: bold; font-size: 13pt");
    ui->n3->setStyleSheet("font-weight: bold; font-size: 13pt");
    ui->n4->setStyleSheet("font-weight: bold; font-size: 13pt");
    ui->n5->setStyleSheet("font-weight: bold; font-size: 13pt");
    ui->n6->setStyleSheet("font-weight: bold; font-size: 13pt");
    ui->n7->setStyleSheet("font-weight: bold; font-size: 13pt");
    ui->n8->setStyleSheet("font-weight: bold; font-size: 13pt");
    ui->n9->setStyleSheet("font-weight: bold; font-size: 13pt");
    ui->n0->setStyleSheet("font-weight: bold; font-size: 13pt");
    ui->eraseBtn->setStyleSheet("font-weight: bold; font-size: 13pt; padding: 7.35px");
    ui->clearBtn->setStyleSheet("font-weight: bold; font-size: 13pt; padding: 7.35px");

    ui->eraseBtn->setText("");
    ui->eraseBtn->setIcon(QIcon(":/resources/backspace.png"));
    ui->clearBtn->setText("");
    ui->clearBtn->setIcon(QIcon(":/resources/x-circle.png"));
}

virtualkeypad::~virtualkeypad()
{
    delete ui;
}

void virtualkeypad::on_clearBtn_clicked()
{
    ui->lineEdit->clear();
    QString text = ui->lineEdit->text();
    global::keyboard::keypadText = text;
}

void virtualkeypad::on_eraseBtn_clicked()
{
    ui->lineEdit->backspace();
    QString text = ui->lineEdit->text();
    global::keyboard::keypadText = text;
}

void virtualkeypad::on_n1_clicked()
{
    ui->lineEdit->insert("1");
    QString text = ui->lineEdit->text();
    global::keyboard::keypadText = text;
}

void virtualkeypad::on_n2_clicked()
{
    ui->lineEdit->insert("2");
    QString text = ui->lineEdit->text();
    global::keyboard::keypadText = text;
}

void virtualkeypad::on_n3_clicked()
{
    ui->lineEdit->insert("3");
    QString text = ui->lineEdit->text();
    global::keyboard::keypadText = text;
}

void virtualkeypad::on_n4_clicked()
{
    ui->lineEdit->insert("4");
    QString text = ui->lineEdit->text();
    global::keyboard::keypadText = text;
}

void virtualkeypad::on_n5_clicked()
{
    ui->lineEdit->insert("5");
    QString text = ui->lineEdit->text();
    global::keyboard::keypadText = text;
}

void virtualkeypad::on_n6_clicked()
{
    ui->lineEdit->insert("6");
    QString text = ui->lineEdit->text();
    global::keyboard::keypadText = text;
}

void virtualkeypad::on_n7_clicked()
{
    ui->lineEdit->insert("7");
    QString text = ui->lineEdit->text();
    global::keyboard::keypadText = text;
}

void virtualkeypad::on_n8_clicked()
{
    ui->lineEdit->insert("8");
    QString text = ui->lineEdit->text();
    global::keyboard::keypadText = text;
}

void virtualkeypad::on_n9_clicked()
{
    ui->lineEdit->insert("9");
    QString text = ui->lineEdit->text();
    global::keyboard::keypadText = text;
}

void virtualkeypad::on_n0_clicked()
{
    ui->lineEdit->insert("0");
    QString text = ui->lineEdit->text();
    global::keyboard::keypadText = text;
}
