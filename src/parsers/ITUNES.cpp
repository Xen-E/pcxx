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

bool Parser::parseITUNES(QFile &fileStream)
{
    struct ITUNES {
        QString name, artist, album, location;
        unsigned long totalTime = 0;
        uint64_t fileSize = 0;
    };
    QVector<ITUNES> tracks;

    QXmlStreamReader xmlReader(&fileStream);
    while (!xmlReader.atEnd() && !xmlReader.hasError()) {

        xmlReader.readNext();
        if (xmlReader.isStartDocument()) {continue;}
        if (xmlReader.isEndDocument()) {break;}


        if (xmlReader.name().toString() == "plist") { //root
            xmlReader.readNextStartElement(); // dict
            xmlReader.readNextStartElement(); // dict

            while (xmlReader.name().toString() != "dict") {
                xmlReader.readNextStartElement();
            }

            while (xmlReader.readNextStartElement()) { //tracks
                while (xmlReader.name().toString() != "dict") {
                    xmlReader.readNextStartElement();
                }
                if (xmlReader.name().toString() == "dict") {
                    ITUNES track;
                    while (xmlReader.readNextStartElement()) { //inside a track
                        const QString eleName(xmlReader.name().toString());
                        if (eleName == "key") {
                            xmlReader.readNext();
                            if (xmlReader.isCharacters() && !xmlReader.isWhitespace()) {
                                const QString keyName(xmlReader.text().toString());

                                if (keyName == "Name") {
                                    xmlReader.skipCurrentElement();
                                    xmlReader.readNextStartElement();
                                    if (xmlReader.name().toString() == "string") {
                                        xmlReader.readNext();
                                        if (xmlReader.isCharacters() && !xmlReader.isWhitespace()) {
                                            track.name = xmlReader.text().toString();
                                        }
                                    }
                                }
                                else if (keyName == "Artist") {
                                    xmlReader.skipCurrentElement();
                                    xmlReader.readNextStartElement();
                                    if (xmlReader.name().toString() == "string") {
                                        xmlReader.readNext();
                                        if (xmlReader.isCharacters() && !xmlReader.isWhitespace()) {
                                            track.artist = xmlReader.text().toString();
                                        }
                                    }
                                }
                                else if (keyName == "Album") {
                                    xmlReader.skipCurrentElement();
                                    xmlReader.readNextStartElement();
                                    if (xmlReader.name().toString() == "string") {
                                        xmlReader.readNext();
                                        if (xmlReader.isCharacters() && !xmlReader.isWhitespace()) {
                                            track.album = xmlReader.text().toString();
                                        }
                                    }
                                }
                                else if (keyName == "Size") {
                                    xmlReader.skipCurrentElement();
                                    xmlReader.readNextStartElement();
                                    if (xmlReader.name().toString() == "integer") {
                                        xmlReader.readNext();
                                        if (xmlReader.isCharacters() && !xmlReader.isWhitespace()) {
                                            track.fileSize = xmlReader.text().toString().toULongLong();
                                        }
                                    }
                                }
                                else if (keyName == "Total Time") {
                                    xmlReader.skipCurrentElement();
                                    xmlReader.readNextStartElement();
                                    if (xmlReader.name().toString() == "integer") {
                                        xmlReader.readNext();
                                        if (xmlReader.isCharacters() && !xmlReader.isWhitespace()) {
                                            track.totalTime = xmlReader.text().toString().toULong();
                                        }
                                    }
                                }
                                else if (keyName == "Location") {
                                    xmlReader.skipCurrentElement();
                                    xmlReader.readNextStartElement();
                                    if (xmlReader.name().toString() == "string") {
                                        xmlReader.readNext();
                                        if (xmlReader.isCharacters() && !xmlReader.isWhitespace()) {
                                            track.location = xmlReader.text().toString();
                                        }
                                    }
                                }
                            }
                        }
                        xmlReader.readNextStartElement();
                    }
                    tracks.push_back(track);
                }

                xmlReader.readNextStartElement();
                xmlReader.readNextStartElement();
            }
            break;
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

                file.title  = track.name;
                file.artist = track.artist;
                file.album  = track.album;

                file.length      = formatLength((track.totalTime / 1000));
                file.rawLength   = (track.totalTime / 1000);
                file.fileSize    = humanizeBytes(track.fileSize);
                file.rawFileSize = track.fileSize;

                AddItemToPlaylist(file);
            }
        }
        else {
            emit log(error, tr("Looks like this XML file either empty or corrupted. No files were found to parse so cancelling the loading process..."));
            return false;
        }
    }
    else {
        emit log(error, tr("Failed to parse the XML data of \"%1\"...Here's why: %2").arg(FileName, xmlReader.errorString()));
        return false;
    }

    return true;
}
