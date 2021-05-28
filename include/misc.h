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


#ifndef MISC_H
#define MISC_H

#include <QString>
#include <QFileInfo>

#include <types.h>

uint64_t fileSizeBytes(const QString &filePath);
const QString humanizeBytes(uint64_t bytes);

bool ID_Exist(unsigned long &ID);
unsigned long generateID();

const QString formatTimer(const qint64 &time);
const QString formatLength(unsigned long length);

bool ExistsInPlaylist(const QString &filePath);
void AddItemToPlaylist(File &file);
bool DeleteItemInPlaylist(const unsigned long &ID);
int GetItemIndexByFilePath(const QString &filePath);

void showInFolder(QString filePath);

QStringList filterDir(const QString &extensions, const QStringList &files, bool caseSensitive);

inline const QString BaseName(const QString &filePath)
{
    QFileInfo FI(filePath);
    return FI.baseName();
}

#endif // MISC_H
