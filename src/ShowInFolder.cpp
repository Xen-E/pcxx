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


#include <misc.h>

#include <QProcess>
#include <QDir>

/**
 * @brief Uses QProcess method to send signals to file managers that support this feature
 * @param filePath File path to show in folder/directory
 */
void showInFolder(QString filePath)
{
    QProcess *process = new QProcess;

    #if defined(__WIN32__) //Uses microsoft windows "explorer" file manager "/select" feature
    process->start("explorer", QStringList() << "/select," << filePath.replace('/', QDir::separator()));

    //This one is tricky and there's gonna be a lot of headaches cuz it's GNU/Linux
    //xdg-open doesn't have a "select" or "highlight" option even after a decade of bitching
    //so i'm gonna try to detect file managers/desktops using the bash
    //nemo (cinnamon) and thunar (xfce) don't have this feature
    //So i'm gonna ignore them and pretend they don't exist :)
    #elif defined(__linux__)
    QProcess *detectDE = new QProcess;
    detectDE->start("sh", QStringList() << "-c" << "echo $XDG_CURRENT_DESKTOP");
    detectDE->waitForFinished();

    const QString DE(detectDE->readAll().trimmed());

    if (!DE.isEmpty() && DE.length() >= 3) {
        if (DE.contains("GNOME")) //Gnome default file manager is Nautilus
            process->start("nautilus", QStringList() << "--select" << filePath);
        else if (DE.contains("KDE")) //KDE Plasma default file manager is Dolphin
            process->start("dolphin", QStringList() << "--select" << filePath);
    }

    #elif defined(__APPLE__) //Default file manager in Darwin is Finder, and it has -R option.
    process->start("open", QStringList() << "-R," << filePath);
    #endif
}
