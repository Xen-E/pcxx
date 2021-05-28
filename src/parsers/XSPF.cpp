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

bool Parser::parseXSPF(QFile &fileStream)
{
    struct XSPF {
        QString location, title, creator, album;
        unsigned long duration = 0;
        uint64_t fileSize = 0;
    };
    QVector<XSPF> tracks;

    QXmlStreamReader xmlReader(&fileStream);
    while (!xmlReader.atEnd() && !xmlReader.hasError()) {

        xmlReader.readNext();
        if (xmlReader.isStartDocument()) {continue;}
        if (xmlReader.isEndDocument()) {break;}


        if (xmlReader.name().toString() == "playlist") { //root
            xmlReader.readNextStartElement(); // trackList

            while (xmlReader.readNextStartElement()) { //Tracks
                XSPF xspf;
                while (xmlReader.readNextStartElement()) { //Inside track element
                    const QString eleName(xmlReader.name().toString());

                    if (eleName == "author" || eleName == "artist" || eleName == "singer" || eleName == "creator") {
                        xmlReader.readNext();
                        if (xmlReader.isCharacters() && !xmlReader.isWhitespace()) {
                            xspf.creator = xmlReader.text().toString();
                        }
                    }
                    else if (eleName == "title") {
                        xmlReader.readNext();
                        if (xmlReader.isCharacters() && !xmlReader.isWhitespace()) {
                            xspf.title = xmlReader.text().toString();
                        }
                    }
                    else if (eleName == "album") {
                        xmlReader.readNext();
                        if (xmlReader.isCharacters() && !xmlReader.isWhitespace()) {
                            xspf.album = xmlReader.text().toString();
                        }
                    }
                    else if (eleName == "length" || eleName == "duration") {
                        xmlReader.readNext();
                        if (xmlReader.isCharacters() && !xmlReader.isWhitespace()) {
                            xspf.duration = xmlReader.text().toString().toULong();
                        }
                    }
                    else if (eleName == "location") {
                        xmlReader.readNext();
                        if (xmlReader.isCharacters() && !xmlReader.isWhitespace()) {
                            xspf.location = xmlReader.text().toString();
                        }
                    }
                    else if (eleName == "filesize" || eleName == "bytes" || eleName == "size") {
                        xmlReader.readNext();
                        if (xmlReader.isCharacters() && !xmlReader.isWhitespace()) {
                            xspf.fileSize = xmlReader.text().toString().toULongLong();
                        }
                    }
                    else xmlReader.skipCurrentElement();

                    xmlReader.readNext();
                }
                tracks.push_back(xspf);
            }
            xmlReader.readNext();
        }
    }

    if (!xmlReader.hasError()) {
        emit log(info, tr("Parsing was successful, Now adding data to the list..."));

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

                if (track.location.isEmpty())
                    continue;

                File file;
                file.ID = generateID();

                file.name     = BaseName(track.location);
                file.fullPath = track.location;

                file.title  = track.title;
                file.artist = track.creator;
                file.album  = track.album;

                file.length      = formatLength(track.duration);
                file.rawLength   = track.duration;
                file.fileSize    = humanizeBytes(track.fileSize);
                file.rawFileSize = track.fileSize;

                AddItemToPlaylist(file);
            }
        }
        else {
            emit log(error, tr("Looks like this XSPF file either empty or corrupted. No files were found to parse so cancelling the loading process..."));
            return false;
        }
    }
    else {
        emit log(error, tr("Failed to parse the XML data of \"%1\"...Here's why: %2").arg(FileName, xmlReader.errorString()));
        return false;
    }

    return true;
}
