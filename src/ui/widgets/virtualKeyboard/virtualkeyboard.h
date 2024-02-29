#ifndef VIRTUALKEYBOARD_H
#define VIRTUALKEYBOARD_H

#include <QWidget>
#include "functions.h"
#include "egg.h"

namespace Ui {
class virtualkeyboard;
}

class virtualkeyboard : public QWidget
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit virtualkeyboard(QWidget *parent = nullptr);
    ~virtualkeyboard();
    bool shift = false;
    bool specialCharacters = false;
    bool embed;
    enum class keyboardMode {
        lowerCase,
        upperCase,
        specialCharacters,
    };
    void reverseKeys(keyboardMode keyboardMode);
    void clearLineEdit();

private slots:
    void on_spaceBtn_clicked();
    void on_eraseBtn_clicked();
    void on_spt_clicked();
    void on_n1_clicked();
    void on_n2_clicked();
    void on_n3_clicked();
    void on_n4_clicked();
    void on_n5_clicked();
    void on_n6_clicked();
    void on_n7_clicked();
    void on_n8_clicked();
    void on_n9_clicked();
    void on_n0_clicked();
    void on_lq_clicked();
    void on_lw_clicked();
    void on_le_clicked();
    void on_lr_clicked();
    void on_lt_clicked();
    void on_ly_clicked();
    void on_lu_clicked();
    void on_li_clicked();
    void on_lo_clicked();
    void on_lp_clicked();
    void on_la_clicked();
    void on_ls_clicked();
    void on_ld_clicked();
    void on_lf_clicked();
    void on_lg_clicked();
    void on_lh_clicked();
    void on_lj_clicked();
    void on_lk_clicked();
    void on_ll_clicked();
    void on_sat_clicked();
    void on_lz_clicked();
    void on_lx_clicked();
    void on_lc_clicked();
    void on_lv_clicked();
    void on_lb_clicked();
    void on_ln_clicked();
    void on_lm_clicked();
    void on_shiftBtn_clicked();
    void adjust_size_function();
    void on_enterBtn_clicked();

    void on_closeBtn_clicked();

private:
    Ui::virtualkeyboard * ui;
    egg * eggWindow;
    keyboardMode currentMode = keyboardMode::lowerCase;

signals:
    void adjust_size();
    void enterBtnPressed(QString string);
    void closeBtnPressed();
};

#endif // VIRTUALKEYBOARD_H
