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

bool Parser::parseM3U(QTextStream &stream)
{
    QStringList artists, titles, filePaths;
    QVector<unsigned long> lengths;

    const QString sig("#EXTINF:");

    while (!stream.atEnd()) {
        const QString line(stream.readLine().trimmed());

        if (line.startsWith(sig)) {
            QString trackInfo(line.mid(line.indexOf(",")+1));
            QStringList data(trackInfo.split("-"));
            QString artist(data[0]);
            QString title(data[1]);

            const unsigned long length(line.split(",")[0].midRef(sig.length()).toULong());

            artists.push_back(trimSpaces(artist));
            titles.push_back(trimSpaces(title));
            lengths.push_back(length);
        }

        if (!line.startsWith("#") && line.length() > 3) {
            filePaths.push_back(line);
        }
    }

    emit log(info, tr("Parsing finished. Now adding data to the list..."));

    if (filePaths.size() > 0) {
        emit setPBMax(filePaths.size());

        for (int f = 0; f < filePaths.size(); f++) {
            if (_CANCEL_LOADING) {
                emit log(warn, tr("Cancel signal has been made, Cancelling loading process..."));
                return false;
            }

            if (Performance > 0)
                QThread::msleep(Performance);

            emit setPB(f);

            File file;
            file.ID = generateID ();

            file.name     = BaseName(filePaths.at(f));
            file.fullPath = filePaths.at(f);

            file.artist   = f < artists.size() ? artists.at(f) : "";
            file.title    = f < titles.size() ? titles.at(f) : "";
            file.album    = "";

            file.fileSize    = humanizeBytes(0);
            file.rawFileSize = 0;
            file.length      = f < lengths.size() ? formatLength(lengths.at(f)) : formatLength(0);
            file.rawLength   = f < lengths.size() ? lengths.at(f) : 0;

            AddItemToPlaylist(file);
        }
    }
    else {
        emit log(error, tr("Hmmm, looks like this M3U file either empty or corrupted. No files were found so cancelling the loading process."));
        return false;
    }

    return true;
}
