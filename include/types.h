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


#ifndef TYPES_H
#define TYPES_H

#include <QString>
#include <QVector>

struct File {
    unsigned long ID;
    QString name, fullPath, fileSize, title, artist, album, length;
    unsigned long rawLength;
    uint64_t rawFileSize;
};
extern QVector<File> Playlist;

enum Format {
    M3U    = 0x1001, //MP3 URL
    PLS    = 0x1002, //museArc/SHOUTcast
    MPCPL  = 0x1003, //Media Player Classic
    AIMPPL = 0x1004, //AIMP
    XSPF   = 0x1005, //XML Shareable Playlist Format
    JSPF   = 0x1006, //JSON Shareable Playlist Format
    WPL    = 0x1007, //Windows Media Player
    ASX    = 0x1008, //Advanced Stream Redirector
    B4S    = 0x1009, //old Winamp ~2.0
    FPL    = 0x1010, //foobar2000
    HTML   = 0x1011, //HTML Document
    TXT    = 0x1012, //Text Document
    ITUNES = 0x1013  //Apple ITunes Media Player
};

enum levelType {
    info  = 0x0030, //Information
    warn  = 0x0031, //Warning
    error = 0x0032, //Error/Critical
    done  = 0x0033 //Finished/Success
};

const QString NEWLINE = "\n";
const QString TABCHAR = "\t";

#endif // TYPES_H
