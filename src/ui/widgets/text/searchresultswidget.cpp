#include "searchresultswidget.h"
#include "ui_searchresultswidget.h"
#include "functions.h"

#include <QMessageBox>

searchResultsWidget::searchResultsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::searchResultsWidget)
{
    ui->setupUi(this);

    // Variables
    libraryResults = false;

    ui->listView->setFont(QFont("u001"));
    ui->listView->setStyleSheet("font-size: 10pt");
    ui->backBtn->setProperty("type", "borderless");
    ui->backBtn->setStyleSheet("font-size: 9pt; padding: 10px; font-weight: bold; background: lightGrey");
    ui->openBtn->setProperty("type", "borderless");
    ui->openBtn->setStyleSheet("font-size: 9pt; padding: 10px; font-weight: bold; background: lightGrey");

    if(global::library::libraryResults == true) {
        global::library::libraryResults = false;
        libraryResults = true;
        ui->openBtn->setText("Get info");
    }
}

searchResultsWidget::~searchResultsWidget()
{
    delete ui;
}

void searchResultsWidget::setListViewContents(QStringList searchResults) {
    QStringListModel * model = new QStringListModel(this);
    model->setStringList(searchResults);
    ui->listView->setModel(model);
}

void searchResultsWidget::on_openBtn_clicked()
{
    if(libraryResults == true) {
        index = ui->listView->currentIndex();
        item = index.data(Qt::DisplayRole).toString();
        if(!item.isEmpty()) {
            // Get currently selected row number
            int selectedRow = ui->listView->currentIndex().row();
            // So that row 0 becomes row 1
            selectedRow = selectedRow + 1;
            QString selectedRowQstr = QString::number(selectedRow);

            QString prog ("sed");
            QStringList args;
            args << "-n" << selectedRowQstr + "p" << "/inkbox/gutenberg-search/search_results_ids";
            QProcess *proc = new QProcess();
            proc->start(prog, args);
            proc->waitForFinished();
            QString bookIdQstr = proc->readAllStandardOutput();
            proc->deleteLater();

            unsigned long bookId = bookIdQstr.toULong();
            global::library::bookId = bookId;

            index = ui->listView->currentIndex();
            item = index.data(Qt::DisplayRole).toString();
            global::library::bookTitle = item;

            BookInfoDialog * bookInfoDialogWindow = new BookInfoDialog();
            connect(bookInfoDialogWindow, SIGNAL(showToast(QString)), SLOT(showToastNative(QString)));
            connect(bookInfoDialogWindow, SIGNAL(closeIndefiniteToast()), SLOT(closeIndefiniteToastNative()));
            connect(bookInfoDialogWindow, SIGNAL(destroyed(QObject*)), SLOT(close()));
            bookInfoDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
            bookInfoDialogWindow->setModal(true);
            emit hideDialog();
            bookInfoDialogWindow->show();

            global::keyboard::searchDialog = false;
            global::keyboard::keyboardDialog = false;
        }
        else {
            emit showToast("Please select a search result");
        }
    }
    else {
        index = ui->listView->currentIndex();
        item = global::localStorage::searchResultsPaths.at(index.row());
        if(!item.isEmpty()) {
            emit openBookFile(item, false);
            global::keyboard::searchDialog = false;
            global::keyboard::keyboardDialog = false;
            searchResultsWidget::close();
        }
        else {
            emit showToast("Please select a search result");
        }
    }
}


void searchResultsWidget::on_backBtn_clicked()
{
    global::forbidOpenSearchDialog = false;
    searchResultsWidget::close();
}

void searchResultsWidget::showToastNative(QString messageToDisplay) {
    emit showToast(messageToDisplay);
}

void searchResultsWidget::closeIndefiniteToastNative() {
    emit closeIndefiniteToast();
}
