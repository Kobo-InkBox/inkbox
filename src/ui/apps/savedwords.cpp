#include "savedwords.h"
#include "functions.h"
#include "ui.h"
#include "ui_savedwords.h"

#include <QFile>
#include <QProcess>
#include <QStringListModel>

SavedWordsAppWidget::SavedWordsAppWidget(QWidget *parent) :
		QWidget(parent),
		ui(new Ui::SavedWordsAppWidget) {
	ui->setupUi(this);
	SavedWordsAppWidget::setFont(QFont("u001"));
	ui->savedWordsLabel->setFont(QFont("Inter"));
	ui->backBtn->setFont(QFont("Inter"));
	ui->clearBtn->setFont(QFont("Inter"));

	UI::applyStyle(*this, "eink");

	ui->backBtn->setProperty("type", "borderless");
	ui->clearBtn->setProperty("type", "borderless");

	ui->savedWordsLabel->setStyleSheet("font-weight: bold");
	ui->wordsList->setStyleSheet("font-size: 10pt");
	ui->backBtn->setStyleSheet("font-weight: bold");
	ui->clearBtn->setStyleSheet("font-weight: bold");

	checkwords();

	QStringListModel *model = new QStringListModel(this);
	QStringList list = words.split("\n", QString::SkipEmptyParts);
	model->setStringList(list);
	ui->wordsList->setModel(model);
	ui->wordsList->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

SavedWordsAppWidget::~SavedWordsAppWidget() {
	delete ui;
}

void SavedWordsAppWidget::on_backBtn_clicked() {
	// Workaround for the random strange memory corruption error
	QProcess process;
	process.startDetached("inkbox", QStringList());
	qApp->quit();
}

void SavedWordsAppWidget::on_clearBtn_clicked() {
	// Warning: possible memory leak here. Though, usually, when you press the "Clear" button and all clears up, you don't have to press it again ;)
	log("Wiping Saved Words list", className);
	save_word_init();
	checkwords();
	QStringListModel *model = new QStringListModel(this);
	QStringList list = words.split("\n", QString::SkipEmptyParts);
	model->setStringList(list);
	ui->wordsList->setModel(model);
	ui->wordsList->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void SavedWordsAppWidget::checkwords() {
	QFile words_list(".config/06-words/config");
	words_list.open(QIODevice::ReadWrite);
	QTextStream in(&words_list);
	words = in.readAll();
	words_list.close();
}

void SavedWordsAppWidget::save_word_init() {
	QFile words(".config/06-words/config");
	words.open(QIODevice::ReadWrite);
	QTextStream in(&words);
	QString words_list = in.readAll();
	string words_list_str = words_list.toStdString();
	words.close();

	ofstream fhandler;
	fhandler.open(".config/06-words/config");
	fhandler << "";
	fhandler.close();
}
