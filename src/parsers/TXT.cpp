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

bool Parser::parseTXT(QTextStream &stream)
{
    QStringList filePaths;

    while (!stream.atEnd()) {
        const QString line(stream.readLine().trimmed());
        if (!line.isEmpty()) filePaths.push_back(line);
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
            file.ID = generateID();

            file.name     = BaseName(filePaths.at(f));
            file.fullPath = filePaths.at(f);

            file.artist = "";
            file.album  = "";

            file.length      = formatLength(0);
            file.rawLength   = 0;

            if (QFile::exists(filePaths[f])) {
                uint64_t fs(fileSizeBytes(filePaths.at(f)));
                file.title       = BaseName(filePaths.at(f));
                file.fileSize    = humanizeBytes(fs);
                file.rawFileSize = fs;
            }
            else {
                file.title       = "";
                file.fileSize    = humanizeBytes(0);
                file.rawFileSize = 0;
            }

            AddItemToPlaylist(file);
        }
    }
    else {
        emit log(error, tr("Hmmm, looks like this Text file either empty or corrupted. No files were found so cancelling the loading process."));
        return false;
    }

    return true;
}
