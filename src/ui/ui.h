#ifndef UI_H
#define UI_H

#include <QString>
#include <QWidget>

/*
 * Utility class for UI related stuff
 */

class UI
{
public:
    UI();

    static void applyStyle(QWidget &widget, QString styleName);

    static void transitionBrightness(int targetValue);
    static void transitionWarmth(int targetValue);
};

#endif // UI_H
