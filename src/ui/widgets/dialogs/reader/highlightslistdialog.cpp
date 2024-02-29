#include "highlightslistdialog.h"
#include "ui_highlightslistdialog.h"

#include <QScreen>

HighlightsListDialog::HighlightsListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HighlightsListDialog)
{
    // Preventing outside interaction
    this->setModal(true);

    ui->setupUi(this);
    ui->listWidget->setFont(QFont("u001"));

    ui->okBtn->setProperty("type", "borderless");
    ui->cancelBtn->setProperty("type", "borderless");
    ui->selectAllBtn->setProperty("type", "borderless");
    ui->okBtn->setStyleSheet("font-size: 9pt; padding: 10px; font-weight: bold; background: lightGrey");
    ui->cancelBtn->setStyleSheet("font-size: 9pt; padding: 10px; font-weight: bold; background: lightGrey");
    ui->selectAllBtn->setStyleSheet("font-size: 9pt; padding: 10px");
    ui->listWidget->setStyleSheet("font-size: 10pt");
    ui->listWidget->setWordWrap(true);

    QJsonObject jsonObject = getHighlightsForBook(global::highlightsListDialog::bookPath);
    int keyCount = 1;

    foreach(const QString& key, jsonObject.keys()) {
        if(keyCount <= 1) {
            keyCount++;
            continue;
        }
        else {
            QListWidgetItem * item = new QListWidgetItem();
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
            item->setCheckState(Qt::Unchecked);
            item->setText(jsonObject.value(key).toString());
            ui->listWidget->addItem(item);
        }
    }

    QTimer::singleShot(150, this, SLOT(increaseSize()));
}

HighlightsListDialog::~HighlightsListDialog()
{
    delete ui;
}

void HighlightsListDialog::increaseSize()
{
    log("Resizing", className);

    ui->listWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    QRect screenGeometry = QGuiApplication::screens()[0]->geometry();
    {
        int wx = screenGeometry.width();
        int x = wx - 25;
        int y = this->height() * determineYIncrease();
        this->setFixedWidth(x);
        this->setFixedHeight(y);
    }
    {
        this->adjustSize();
        int x = (screenGeometry.width() - this->width()) / 2;
        int y = (screenGeometry.height() - this->height()) / 2;
        this->move(x, y);
    }

    this->show();
}

void HighlightsListDialog::on_cancelBtn_clicked()
{
    HighlightsListDialog::close();
}

void HighlightsListDialog::on_okBtn_clicked()
{
    int itemsCount = 0;
    int range = ui->listWidget->count();
    for(int i = 0; i < range; i++) {
        if(ui->listWidget->item(i)->checkState() == Qt::Checked) {
            QString highlight = ui->listWidget->item(i)->text();
            // The last argument indicates that we want to remove the specified highlight, not add it
            highlightBookText(highlight, global::highlightsListDialog::bookPath, true);
            itemsCount++;
        }
    }

    global::toast::delay = 3000;
    if(itemsCount >= 1) {
        if(itemsCount >= 2) {
            emit showToast("Items deleted successfully");
        }
        else {
            emit showToast("Item deleted successfully");
        }
        HighlightsListDialog::close();
    }
    else {
        emit showToast("Please select items to delete");
    }
}

void HighlightsListDialog::on_selectAllBtn_clicked()
{
    int range = ui->listWidget->count();
    for(int i = 0; i < range; i++) {
        ui->listWidget->item(i)->setCheckState(Qt::Checked);
    }
}

void HighlightsListDialog::on_listWidget_itemClicked(QListWidgetItem *item)
{
    if(item->checkState() == Qt::Unchecked) {
        item->setCheckState(Qt::Checked);
    }
    else {
        item->setCheckState(Qt::Unchecked);
    }
}
