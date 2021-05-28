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


#include <parser.h>

bool Parser::parseAIMPPL(QTextStream &stream)
{
    struct AIMPPL {
        QString filePath, Title, Artist, Album;
        unsigned long length = 0;
        uint64_t fileSize = 0;
    };
    QVector<AIMPPL> tracks;

    while (!stream.atEnd()) {
        const QString line(stream.readLine().trimmed());
        if (line.startsWith("#") || line.startsWith("-"))
            continue;

        const QStringList data(line.split("|"));

        if (data.length() < 16)
            continue;

        AIMPPL pl;

        pl.filePath = data.at(0).isEmpty() ? QString() : data.at(0);
        pl.Title    = data.at(1).isEmpty() ? QString() : data.at(1);
        pl.Artist   = data.at(2).isEmpty() ? QString() : data.at(2);
        pl.Album    = data.at(3).isEmpty() ? QString() : data.at(3);

        pl.length   = data.at(14).isEmpty() ? 0 : (data.at(14).toULong() / 1000);
        pl.fileSize = data.at(15).isEmpty() ? 0 : data.at(15).toULongLong();

        tracks.push_back(pl);
    }

    emit log(info, tr("Parsing finished. Now adding data to the list..."));

    if (tracks.size() > 0) {
        emit setPBMax(tracks.size());

        for (auto &track : tracks) {
            int index((&track - tracks.begin()) + 1);

            if (_CANCEL_LOADING) {
                emit log(warn, tr("Cancel signal has been made, Cancelling loading process..."));
                return false;
            }

            if (Performance > 0)
                QThread::msleep(Performance);

            emit setPB(index);

            if (track.filePath == "N/A")
                continue;

            File file;
            file.ID = generateID();

            file.name     = BaseName(track.filePath);
            file.fullPath = track.filePath;

            file.title  = track.Title;
            file.artist = track.Artist;
            file.album  = track.Album;

            file.length      = formatLength(track.length);
            file.fileSize    = humanizeBytes(track.fileSize);
            file.rawLength   = track.length;
            file.rawFileSize = track.fileSize;

            AddItemToPlaylist(file);
        }

    }
    else {
        emit log(error, tr("Hmmm, looks like this AIMPPL file either empty or corrupted. No files were found so cancelling the loading process."));
        return false;
    }

    return true;
}
