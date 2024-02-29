#ifndef READER_H
#define READER_H

#include "functions.h"
#include "splashAlert.h"
#include "generaldialog.h"
#include "toast.h"
#include "quit.h"
#include "textdialog.h"
#include "highlightslistdialog.h"
#include "brightnessdialog.h"

#include <QWidget>
#include <QGraphicsScene>

using namespace std;

// ePUB & PDF scaling
namespace mupdf {
    inline bool convertRelativeValues;
    namespace epub {
        inline int fontSize;
        inline int width;
        inline int height;
        inline int epubPageNumber;
        inline QString fontSize_qstr;
        inline QString width_qstr;
        inline QString height_qstr;
        inline QString epubPageNumber_qstr;
    }
    namespace pdf {
        inline int width;
        inline int height;
        inline int relativeWidth;
        inline int relativeHeight;
        inline int pdfPageNumber;
        inline QString width_qstr;
        inline QString height_qstr;
        inline QString pdfPageNumber_qstr;
    }
}

namespace Ui {
class ReaderWidget;
}

class ReaderWidget : public QWidget
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit ReaderWidget(QWidget *parent = nullptr);
    ~ReaderWidget();

    int split_total;
    int split_files_number;
    int page_number;
    int dictionary_position = 1;
    int pagesTurned = 0;

    // -1 : Never refresh | 0 : Refresh every page | 1 : Refresh every 1 page. And so on...
    // Refresh every 3 pages is the default
    int pageRefreshSetting = 3;

    bool menubar_shown = false;
    bool nextdefinition_lock = false;
    bool is_epub = false;
    bool is_pdf = false;
    bool is_image = false;
    bool parser_ran = false;
    bool filematch_ran = false;
    bool neverRefresh = false;
    bool wakeFromSleep = false;
    bool remount = true;
    bool showTopbarWidget;
    bool wordwidgetLock;
    bool textDialogLock;
    bool isNightModeActive;
    bool goToSavedPageDone;
    QString ittext;
    QString book_file;
    QString selected_text;
    QString word;
    QString words; // Saved words
    QString letter;
    string selected_text_str;
    QString definition;
    QPixmap scaledChargingPixmap;
    QPixmap scaledHalfPixmap;
    QPixmap scaledFullPixmap;
    QPixmap scaledEmptyPixmap;
    QList<QString> content;
    QString epubPageContent;
    QString usbmsStatus;
    int pageNumberInt;
    int totalPagesInt;
    int pdfOrientation;
    bool initialPdfRotationDone;

    int setup_book(QString book, int i, bool run_parser);
    void checkwords();
    bool epub_file_match(QString file);
    void dictionary_lookup(string word, QString first_letter, int position);
    void save_word(string word, bool remove);
    void menubar_show();
    void menubar_hide();
    void wordwidget_show();
    void wordwidget_hide();
    void openLowBatteryDialog();
    void openCriticalBatteryAlertWindow();
    void convertMuPdfVars(int fileType, bool convertAll);
    void refreshScreen();
    void setPageStyle(int fileType);
    void setTextProperties(int alignment, int lineSpacing, int margins, QString font, int fontSize);
    void delay(int seconds);
    void openUsbmsDialog();
    QString setPageNumberLabelContent();
    void setupPageWidget();
    void getTotalEpubPagesNumber();
    void getTotalPdfPagesNumber();
    bool pdf_file_match(QString file);
    bool image_file_match(QString file);

private slots:
    void on_nextBtn_clicked();
    void on_previousBtn_clicked();
    void on_optionsBtn_clicked();
    void on_homeBtn_clicked();
    void on_fontChooser_currentIndexChanged(const QString &arg1);
    void on_alignLeftBtn_clicked();
    void on_alignCenterBtn_clicked();
    void on_alignRightBtn_clicked();
    void on_alignJustifyBtn_clicked();
    void on_infoCloseBtn_clicked();
    void on_previousDefinitionBtn_clicked();
    void on_nextDefinitionBtn_clicked();
    void on_saveWordBtn_clicked();
    void on_sizeSlider_valueChanged(int value);
    void writeconfig_pagenumber(bool persistent);
    void quit_restart();
    void on_text_selectionChanged();
    void on_nightModeBtn_clicked();
    void on_gotoBtn_clicked();
    void gotoPage(int pageNumber);
    void on_searchBtn_clicked();
    void searchRefreshScreen();
    void setupSearchDialog();
    void saveReadingSettings();
    void setupLocalSettingsEnvironment();
    void setupPng();
    void on_pdfScaleSlider_valueChanged(int value);
    void on_decreaseScaleBtn_clicked();
    void on_increaseScaleBtn_clicked();
    void openBookFileNative(QString book, bool relativePath);
    void showToast(QString messageToDisplay);
    void on_quitBtn_clicked();
    void closeIndefiniteToast();
    void getPdfOrientation(QString file);
    void unsetTextDialogLock();
    void highlightText();
    void unhighlightText();
    void on_viewHighlightsBtn_clicked();
    void setTextPropertiesSlot();
    void setInitialTextProperties();
    void setLineSpacing(int spacing, bool write = true);
    void setMargins(int margins, bool write = true);
    void on_lineSpacingSlider_valueChanged(int value);
    void on_marginsSlider_valueChanged(int value);
    void on_brightnessBtn_clicked();
    void setCinematicBrightnessWarmthSlot();

signals:
    void openBookFile(QString book, bool relativePath);

private:
    Ui::ReaderWidget * ui;
    SplashAlertWidget * alertWindow;
    GeneralDialog * generalDialogWindow;
    toast * toastWindow;
    QuitWidget * quitWindow;
    TextDialog * textDialogWindow;
    HighlightsListDialog * highlightsListDialogWindow;
    QGraphicsScene * graphicsScene;
    BrightnessDialog * brightnessDialogWindow;
};

#endif // READER_H
