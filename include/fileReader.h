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


#ifndef FILEREADER_H
#define FILEREADER_H

#include <types.h>
#include <misc.h>

#include <QStringList>
#include <QThread>
#include <QElapsedTimer>

#include <taglib/tag.h>
#include <taglib/fileref.h>

class FileReader : public QObject
{
    Q_OBJECT

public:
    QString dir;
    QStringList files;
    int Performance;
    bool ShowMsgWhenDone;

    virtual ~FileReader() { delete this; }

public slots:
    void Read();

signals:
    void log(levelType type, const QString &log);
    void ShowMessageBox(levelType type, const QString &title, const QString &msg);
    void setPBMax(const int value);
    void setPB(const int value);
    void disableUI(const bool really);
    void UpdateStats();
    void UpdateFilesList();

};

extern bool _CANCEL_LOADING;
extern bool _UNEXPORTED;

#endif // FILEREADER_H
