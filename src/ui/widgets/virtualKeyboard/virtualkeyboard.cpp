#include "virtualkeyboard.h"
#include "ui_virtualkeyboard.h"
#include "device.h"

#include <QTimer>
#include <QScreen>
#include <QDebug>

virtualkeyboard::virtualkeyboard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::virtualkeyboard)
{   
    Device* device = Device::getSingleton();

    Device::ScreenSize size = device->getScreenSize();
    QString padding = "";
    if(size == Device::ScreenSize::SMALL) {
        padding = "11.45px";
    } else if(size == Device::ScreenSize::MEDIUM) {
        padding = "13px";
    } else if(size == Device::ScreenSize::LARGE) {
        padding = "20px";
    } else {
        padding = "27px";
    }

    ui->setupUi(this);
    this->setStyleSheet(readFile("/mnt/onboard/.adds/inkbox/eink.qss"));

    if(global::keyboard::embed == false) {
        global::keyboard::embed = true;
        embed = false;
        ui->leftSpacerWidget->hide();
        ui->rightSpacerWidget->hide();
        ui->leftSpacerWidget->deleteLater();
        ui->rightSpacerWidget->deleteLater();
        ui->closeBtn->setProperty("type", "borderless");
        ui->enterBtn->setProperty("type", "borderless");
        {
            ui->closeBtn->setStyleSheet("font-weight: bold; font-size: 9pt; padding: " + padding);
            ui->closeBtn->setIcon(QIcon(":/resources/close.png"));
            ui->enterBtn->setStyleSheet("font-weight: bold; font-size: 9pt; padding: " + padding);
            ui->enterBtn->setIcon(QIcon(":/resources/arrow-right.png"));
        }
    }
    else {
        embed = true;
        ui->line->hide();
        ui->line->deleteLater();
        ui->closeBtn->hide();
        ui->closeBtn->deleteLater();
        ui->enterBtn->hide();
        ui->enterBtn->deleteLater();
    }
    if(!global::keyboard::keyboardText.isEmpty()) {
        ui->lineEdit->setText(global::keyboard::keyboardText);
    }

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

    ui->la->setProperty("type", "borderless");
    ui->lb->setProperty("type", "borderless");
    ui->lc->setProperty("type", "borderless");
    ui->ld->setProperty("type", "borderless");
    ui->le->setProperty("type", "borderless");
    ui->lf->setProperty("type", "borderless");
    ui->lg->setProperty("type", "borderless");
    ui->lh->setProperty("type", "borderless");
    ui->li->setProperty("type", "borderless");
    ui->lj->setProperty("type", "borderless");
    ui->lk->setProperty("type", "borderless");
    ui->ll->setProperty("type", "borderless");
    ui->lm->setProperty("type", "borderless");
    ui->ln->setProperty("type", "borderless");
    ui->lo->setProperty("type", "borderless");
    ui->lp->setProperty("type", "borderless");
    ui->lq->setProperty("type", "borderless");
    ui->lr->setProperty("type", "borderless");
    ui->ls->setProperty("type", "borderless");
    ui->lt->setProperty("type", "borderless");
    ui->lu->setProperty("type", "borderless");
    ui->lv->setProperty("type", "borderless");
    ui->lw->setProperty("type", "borderless");
    ui->lx->setProperty("type", "borderless");
    ui->ly->setProperty("type", "borderless");
    ui->lz->setProperty("type", "borderless");

    ui->eraseBtn->setProperty("type", "borderless");
    ui->shiftBtn->setProperty("type", "borderless");
    ui->spt->setProperty("type", "borderless");
    ui->sat->setProperty("type", "borderless");

    ui->n1->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->n2->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->n3->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->n4->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->n5->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->n6->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->n7->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->n8->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->n9->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->n0->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);

    ui->la->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->lb->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->lc->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->ld->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->le->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->lf->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->lg->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->lh->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->li->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->lj->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->lk->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->ll->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->lm->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->ln->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->lo->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->lp->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->lq->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->lr->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->ls->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->lt->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->lu->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->lv->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->lw->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->lx->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->ly->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->lz->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);

    ui->eraseBtn->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->shiftBtn->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->spt->setStyleSheet("font-weight: bold; font-size: 9pt; padding: "+padding);
    ui->sat->setStyleSheet("font-weight: bold; font-size: 7pt; padding: "+padding);
    ui->spaceBtn->setStyleSheet("font-weight: bold; font-size: 9pt; padding: 10px; border: 1px solid black");


    ui->lineEdit->setStyleSheet("border: 3px solid black");
    if(global::keyboard::vncDialog == true or global::keyboard::wifiPassphraseDialog == true or global::keyboard::encfsDialog == true) {
        ui->lineEdit->setFont(QFont("Roboto Mono"));
    }
    else {
        ui->lineEdit->setFont(QFont("u001"));
    }

    if(embed == false) {
        adjust_size_function();
    }
}

virtualkeyboard::~virtualkeyboard()
{
    delete ui;
}

void virtualkeyboard::on_spaceBtn_clicked()
{
    ui->lineEdit->insert(" ");
    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_eraseBtn_clicked()
{
    ui->lineEdit->backspace();
    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_spt_clicked()
{
    ui->lineEdit->insert(ui->spt->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_sat_clicked()
{
    if(specialCharacters == true) {
        specialCharacters = false;
        reverseKeys(keyboardMode::lowerCase);
    }
    else {
        reverseKeys(keyboardMode::specialCharacters);
        specialCharacters = true;
    }
}

void virtualkeyboard::on_n1_clicked()
{
    ui->lineEdit->insert(ui->n1->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_n2_clicked()
{
    ui->lineEdit->insert(ui->n2->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_n3_clicked()
{
    ui->lineEdit->insert(ui->n3->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_n4_clicked()
{
    ui->lineEdit->insert(ui->n4->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_n5_clicked()
{
    ui->lineEdit->insert(ui->n5->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_n6_clicked()
{
    ui->lineEdit->insert(ui->n6->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_n7_clicked()
{
    ui->lineEdit->insert(ui->n7->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_n8_clicked()
{
    ui->lineEdit->insert(ui->n8->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_n9_clicked()
{
    ui->lineEdit->insert(ui->n9->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_n0_clicked()
{
    ui->lineEdit->insert(ui->n0->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_lq_clicked()
{
    ui->lineEdit->insert(ui->lq->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_lw_clicked()
{
    ui->lineEdit->insert(ui->lw->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_le_clicked()
{
    ui->lineEdit->insert(ui->le->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_lr_clicked()
{
    ui->lineEdit->insert(ui->lr->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_lt_clicked()
{
    ui->lineEdit->insert(QString(ui->lt->text().back()));

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_ly_clicked()
{
    ui->lineEdit->insert(ui->ly->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_lu_clicked()
{
    ui->lineEdit->insert(ui->lu->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_li_clicked()
{
    ui->lineEdit->insert(ui->li->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_lo_clicked()
{
    ui->lineEdit->insert(ui->lo->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_lp_clicked()
{
    ui->lineEdit->insert(ui->lp->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_la_clicked()
{
    ui->lineEdit->insert(ui->la->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_ls_clicked()
{
    ui->lineEdit->insert(ui->ls->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_ld_clicked()
{
    ui->lineEdit->insert(ui->ld->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_lf_clicked()
{
    ui->lineEdit->insert(ui->lf->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_lg_clicked()
{
    ui->lineEdit->insert(ui->lg->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_lh_clicked()
{
    ui->lineEdit->insert(ui->lh->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_lj_clicked()
{
    ui->lineEdit->insert(ui->lj->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_lk_clicked()
{
    ui->lineEdit->insert(ui->lk->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_ll_clicked()
{
    ui->lineEdit->insert(ui->ll->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_lz_clicked()
{
    ui->lineEdit->insert(ui->lz->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_lx_clicked()
{
    ui->lineEdit->insert(ui->lx->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_lc_clicked()
{
    ui->lineEdit->insert(ui->lc->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_lv_clicked()
{
    ui->lineEdit->insert(ui->lv->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_lb_clicked()
{
    ui->lineEdit->insert(ui->lb->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_ln_clicked()
{
    ui->lineEdit->insert(ui->ln->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_lm_clicked()
{
    ui->lineEdit->insert(ui->lm->text());

    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_shiftBtn_clicked()
{
    if(shift == true) {
        shift = false;
        reverseKeys(keyboardMode::lowerCase);
    }
    else {
        reverseKeys(keyboardMode::upperCase);
        shift = true;
    }

}

void virtualkeyboard::reverseKeys(keyboardMode keyboardMode) {
    currentMode = keyboardMode;
    if(keyboardMode == keyboardMode::upperCase) {
        ui->shiftBtn->setText("⇪");
        ui->n1->setText("1");
        ui->n2->setText("2");
        ui->n3->setText("3");
        ui->n4->setText("4");
        ui->n5->setText("5");
        ui->n6->setText("6");
        ui->n7->setText("7");
        ui->n8->setText("8");
        ui->n9->setText("9");
        ui->n0->setText("0");

        ui->la->setText("A");
        ui->lb->setText("B");
        ui->lc->setText("C");
        ui->ld->setText("D");
        ui->le->setText("E");
        ui->lf->setText("F");
        ui->lg->setText("G");
        ui->lh->setText("H");
        ui->li->setText("I");
        ui->lj->setText("J");
        ui->lk->setText("K");
        ui->ll->setText("L");
        ui->lm->setText("M");
        ui->ln->setText("N");
        ui->lo->setText("O");
        ui->lp->setText("P");
        ui->lq->setText("Q");
        ui->lr->setText("R");
        ui->ls->setText("S");
        ui->lt->setText("T");
        ui->lu->setText("U");
        ui->lv->setText("V");
        ui->lw->setText("W");
        ui->lx->setText("X");
        ui->ly->setText("Y");
        ui->lz->setText("Z");
    }
    else if(keyboardMode == keyboardMode::lowerCase) {
        ui->shiftBtn->setText("⇧");

        ui->n1->setText("1");
        ui->n2->setText("2");
        ui->n3->setText("3");
        ui->n4->setText("4");
        ui->n5->setText("5");
        ui->n6->setText("6");
        ui->n7->setText("7");
        ui->n8->setText("8");
        ui->n9->setText("9");
        ui->n0->setText("0");

        ui->la->setText("a");
        ui->lb->setText("b");
        ui->lc->setText("c");
        ui->ld->setText("d");
        ui->le->setText("e");
        ui->lf->setText("f");
        ui->lg->setText("g");
        ui->lh->setText("h");
        ui->li->setText("i");
        ui->lj->setText("j");
        ui->lk->setText("k");
        ui->ll->setText("l");
        ui->lm->setText("m");
        ui->ln->setText("n");
        ui->lo->setText("o");
        ui->lp->setText("p");
        ui->lq->setText("q");
        ui->lr->setText("r");
        ui->ls->setText("s");
        ui->lt->setText("t");
        ui->lu->setText("u");
        ui->lv->setText("v");
        ui->lw->setText("w");
        ui->lx->setText("x");
        ui->ly->setText("y");
        ui->lz->setText("z");
    }
    else if(keyboardMode == keyboardMode::specialCharacters) {
        ui->shiftBtn->setText("⇧");

        ui->n1->setText("1");
        ui->n2->setText("2");
        ui->n3->setText("3");
        ui->n4->setText("4");
        ui->n5->setText("5");
        ui->n6->setText("6");
        ui->n7->setText("7");
        ui->n8->setText("8");
        ui->n9->setText("9");
        ui->n0->setText("0");

        ui->lq->setText("@");
        ui->lw->setText("#");
        ui->le->setText("€");
        ui->lr->setText("%");
        // Important
        ui->lt->setText("&&");
        ui->ly->setText("-");
        ui->lu->setText("+");
        ui->li->setText("(");
        ui->lo->setText(")");
        ui->lp->setText("*");
        // Important
        ui->la->setText("\"");
        ui->ls->setText("'");
        ui->ld->setText(":");
        ui->lf->setText(";");
        ui->lg->setText("!");
        ui->lh->setText("?");
        ui->lj->setText(",");
        ui->lk->setText("_");
        ui->ll->setText("/");
        ui->lz->setText("~");

        ui->lx->setText("{");
        ui->lc->setText("}");
        ui->lv->setText("[");
        ui->lb->setText("]");
        ui->ln->setText("<");
        ui->lm->setText(">");
    }
    QTimer::singleShot(1000, this, SLOT(adjust_size_function()));
}

void virtualkeyboard::adjust_size_function() {
    if(embed == true) {
        emit adjust_size();
    }
    else {
        this->setFixedHeight(QGuiApplication::screens()[0]->size().height() * 50 / 100);
        this->setFixedWidth(QGuiApplication::screens()[0]->size().width());
        this->move(0, (QGuiApplication::screens()[0]->size().height() - this->height()));
    }
}

void virtualkeyboard::clearLineEdit() {
    ui->lineEdit->clear();
    QString text = ui->lineEdit->text();
    global::keyboard::keyboardText = text;
}

void virtualkeyboard::on_enterBtn_clicked()
{
    if(global::keyboard::keyboardText == "sudo rm -rf /" or global::keyboard::keyboardText == "rm -rf /") {
        egg * eggWindow = new egg();
        eggWindow->setAttribute(Qt::WA_DeleteOnClose);
        eggWindow->showFullScreen();
    }
    else {
        global::keyboard::keyboardText = ui->lineEdit->text();
        emit enterBtnPressed(global::keyboard::keyboardText);
        this->close();
    }
}

void virtualkeyboard::on_closeBtn_clicked()
{
    global::keyboard::keyboardText = "";
    emit closeBtnPressed();
    this->close();
}
