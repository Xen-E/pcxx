/*******************************************************************************************
*    Copyright (c) 2019 Xen <xen-dev@pm.me>                                                *
*    This file is part of the Playlist Converter++ project, AKA PC++ AKA PCxx.             *
*    Playlist Converter++ is free software; you can redistribute it and/or modify it under *
*    the terms of the GNU Lesser General Public License (LGPL) as published                *
*    by the Free Software Foundation; either version 3 of the License, or                  *
*    (at your option) any later version.                                                   *
*    Playlist Converter++ is distributed in the hope that it will be useful, but WITHOUT   *
*    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or                 *
*    FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public                   *
*    License for more details.                                                             *
*    You should have received a copy of the GNU Lesser General Public License              *
*    along with this program. If not, see <https://www.gnu.org/licenses>.                  *
*******************************************************************************************/


#include "pcxx.h"
#include "versions.h"

#include <QApplication>
#include <QStyleFactory>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setOrganizationName(Developer::name);
    app.setOrganizationDomain(Developer::domain);
    app.setApplicationName(Product::name);
    app.setApplicationVersion(Product::version);
    app.setWindowIcon(QIcon(":/icons/pcxx.png"));

    qApp->setStyle(QStyleFactory::create("Fusion"));

    QSettings settings;

    QString style(
    "QGroupBox { border: none; }"
    "QProgressBar { border: 1px solid #808080; border-radius: 5px; }"
    "QSlider::groove:horizontal { height: 4px; border-radius: 5px; background-color: qlineargradient(spread:pad, x1:0, y1:1, x2:1, y2:1, stop:0 rgba(255, 0, 0, 255), stop:0.501742 rgba(251, 255, 0, 255), stop:0.912892 rgba(10, 255, 10, 255)); }"
    "QSlider::handle:horizontal { border: 1px solid #262626; background-color: #f2f2f2; width: 16px; margin: -3 0; }"
    );

    if (settings.value("EyeCandy/ThemeMode", 1).toInt() == 1) {
        QColor darkGray(53, 53, 53);
        QColor gray(128, 128, 128);
        QColor black(25, 25, 25);
        QColor blue(42, 130, 218);

        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, darkGray);
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, black);
        darkPalette.setColor(QPalette::AlternateBase, darkGray);
        darkPalette.setColor(QPalette::ToolTipBase, blue);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Button, darkGray);
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::Link, blue);
        darkPalette.setColor(QPalette::Highlight, blue);
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);


        darkPalette.setColor(QPalette::Active, QPalette::Button, gray.darker());
        darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, gray);
        darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, gray);
        darkPalette.setColor(QPalette::Disabled, QPalette::Text, gray);
        darkPalette.setColor(QPalette::Disabled, QPalette::Light, darkGray);
        darkPalette.setColor(QPalette::Disabled, QPalette::Highlight, darkGray);

        qApp->setPalette(darkPalette);

        style += "QToolBox { background-color: #353535; }"
        "QToolBox::tab { background: url(\":icons/right-arrow.png\") no-repeat #808080; border-radius: 8px; font: bold; padding-left: 25px; }"
        "QToolBox::tab:selected { background: url(\":icons/down-arrow.png\") no-repeat #5e5e5e; font: italic; padding-left: 25px; }"
        "QToolBox QScrollArea>QWidget>QWidget { background-color: #353535; }"
        "QLineEdit:disabled { background-color: #808080; color: #333333; }"
        "QToolTip { color: #ffffff; background-color: #808080; border: 1px solid white; }";
    }

    switch (settings.value("EyeCandy/PBTheme", 3).toInt()) {
    case 1: //Ocean
        style += "QProgressBar::chunk { background: url(\":icons/pb_bg_blue.png\"); color: #ffffff; }";
        break;
    case 2: //Sky
        style += "QProgressBar::chunk { background: url(\":icons/pb_bg_light_blue.png\"); color: #ffffff; }";
        break;
    case 3: //Girly
        style += "QProgressBar::chunk { background: url(\":icons/pb_bg_pink.png\"); color: #ffffff; }";
        break;
    case 4: //Carbon
        style += "QProgressBar::chunk { background: url(\":icons/pb_bg.png\"); color: #ffffff; }";
        break;
    }

    qApp->setStyleSheet(style);

    PCxx w;
    w.show();

    return app.exec();
}
