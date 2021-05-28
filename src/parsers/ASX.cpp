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

bool Parser::parseASX(QFile &fileStream)
{

    struct ASX {
        QString filePath, title, artist, album;
        unsigned long length = 0;
        uint64_t fileSize = 0;
    };
    QVector<ASX> tracks;

    QXmlStreamReader xmlReader(&fileStream);
    while (!xmlReader.atEnd() && !xmlReader.hasError()) {

        xmlReader.readNext();
        if (xmlReader.isStartDocument()) {continue;}
        if (xmlReader.isEndDocument()) {break;}


        if (xmlReader.name().toString() == "asx") { //root

            while (xmlReader.readNextStartElement()) { //Entries

                ASX asx;
                while (xmlReader.readNextStartElement()) { //Inside entry element
                    const QString eleName(xmlReader.name().toString());

                    if (eleName == "author" || eleName == "artist" || eleName == "singer" || eleName == "creator") {
                        xmlReader.readNext();
                        if (xmlReader.isCharacters() && !xmlReader.isWhitespace()) {
                            asx.artist = xmlReader.text().toString();
                        }
                    }
                    else if (eleName == "title") {
                        xmlReader.readNext();
                        if (xmlReader.isCharacters() && !xmlReader.isWhitespace()) {
                            asx.title = xmlReader.text().toString();
                        }
                    }
                    else if (eleName == "album") {
                        xmlReader.readNext();
                        if (xmlReader.isCharacters() && !xmlReader.isWhitespace()) {
                            asx.album = xmlReader.text().toString();
                        }
                    }
                    else if (eleName == "length" || eleName == "duration") {
                        QXmlStreamAttributes attrs(xmlReader.attributes());
                        if (attrs.hasAttribute("value")) {
                            asx.length = attrs.value("value").toString().toULong();
                        }
                    }
                    else if (eleName == "filesize" || eleName == "bytes" || eleName == "size") {
                        xmlReader.readNext();
                        if (xmlReader.isCharacters() && !xmlReader.isWhitespace()) {
                            asx.fileSize = xmlReader.text().toString().toULongLong();
                        }
                    }
                    else if (eleName == "ref") {
                        QXmlStreamAttributes attrs(xmlReader.attributes());
                        if (attrs.hasAttribute("href"))
                            asx.filePath = attrs.value("href").toString();
                        else asx.filePath = QString();

                    }
                    else xmlReader.skipCurrentElement();

                    xmlReader.readNext();

                }
                tracks.push_back(asx);
            }
        }
    }

    if (!xmlReader.hasError()) {
        emit log(info, tr("Parsing was successful. Now adding data to the list..."));

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

                if (track.filePath.isEmpty())
                    continue;

                File file;
                file.ID = generateID();

                file.name     = BaseName(track.filePath);
                file.fullPath = track.filePath;

                file.title  = track.title;
                file.artist = track.artist;
                file.album  = track.album;

                file.length      = formatLength(track.length);
                file.rawLength   = track.length;
                file.fileSize    = humanizeBytes(track.fileSize);
                file.rawFileSize = track.fileSize;

                AddItemToPlaylist(file);
            }
        }
        else {
            emit log(error, tr("Looks like this ASX file either empty or corrupted. No files were found to parse so cancelling the loading process..."));
            return false;
        }
    }
    else {
        emit log(error, tr("Failed to parse the XML data of \"%1\"...Here's why: %2").arg(FileName, xmlReader.errorString()));
        return false;
    }

    return true;

}
