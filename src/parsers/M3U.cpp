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
    QVector<uint64_t> fileSizes;

    const QString STANDARD_SIG("#EXTINF:");
    const QString VIRTUALDJ_SIG("#EXTVDJ:");

    while (!stream.atEnd()) {
        const QString line(stream.readLine().trimmed());

        if (line.startsWith(STANDARD_SIG)) {
            QString trackInfo(line.mid(line.indexOf(",")+1));
            QStringList data(trackInfo.split("-"));
            QString artist(data[0]);
            QString title(data[1]);

            const unsigned long length(line.split(",")[0].midRef(STANDARD_SIG.length()).toULong());

            artists.push_back(trimSpaces(artist));
            titles.push_back(trimSpaces(title));
            lengths.push_back(length);
        }
        /*
         * This is for VirtualDJ M3U playlists format
         * it uses XML tags in the track information line
         * which is contain its signature (#EXTVDJ)
         * it also stores file size and that don't exist in
         * normal standard M3U/M3U8 playlists format.
        */
        else if (line.startsWith(VIRTUALDJ_SIG)) {
            if (line.contains("<filesize>")) {
                const int fs_POS(line.indexOf("<filesize>")+10);
                const int fs_LENGTH(line.indexOf("</filesize>")-fs_POS);
                fileSizes.push_back(line.midRef(fs_POS, fs_LENGTH).toULongLong());
            }
            else fileSizes.push_back(0);

            if (line.contains("<artist>")) {
                const int a_POS(line.indexOf("<artist>")+8);
                const int a_LENGTH(line.indexOf("</artist>")-a_POS);
                artists.push_back(line.mid(a_POS, a_LENGTH));
            }
            else artists.push_back("");

            if (line.contains("<title>")) {
                const int t_POS(line.indexOf("<title>")+7);
                const int t_LENGTH(line.indexOf("</title>")-t_POS);
                titles.push_back(line.mid(t_POS, t_LENGTH));
            }
            else titles.push_back("");

            if (line.contains("<songlength>")) {
                const int sl_POS(line.indexOf("<songlength>")+12);
                const int sl_LENGTH(line.indexOf("</songlength>")-sl_POS);
                lengths.push_back(line.midRef(sl_POS, sl_LENGTH).toDouble()); //toDouble is important since VDJ uses float value.
            }
            else lengths.push_back(0);
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

            file.fileSize    = f < fileSizes.size() ? humanizeBytes(fileSizes.at(f)) : humanizeBytes(0);
            file.rawFileSize = f < fileSizes.size() ? fileSizes.at(f) : 0;
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
