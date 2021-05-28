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

bool Parser::parseB4S(QFile &fileStream)
{
    struct B4S {
        QString filePath, title;
    };
    QVector<B4S> tracks;

    QXmlStreamReader xmlReader(&fileStream);
    while (!xmlReader.atEnd() && !xmlReader.hasError()) {

        xmlReader.readNext();
        if (xmlReader.isStartDocument()) {continue;}
        if (xmlReader.isEndDocument()) {break;}


        if (xmlReader.name().toString() == "WinampXML") { //root

            while (xmlReader.readNextStartElement()) { //Entries
                while (xmlReader.readNextStartElement()) { //Entry
                    B4S b4s;
                    if (xmlReader.name().toString() == "entry") {
                        QXmlStreamAttributes attrs(xmlReader.attributes());
                        b4s.filePath = attrs.value("Playstring").toString();

                        xmlReader.readNextStartElement();

                        if (xmlReader.name().toString() == "Name") { //Name element
                            xmlReader.readNext();
                            if (xmlReader.isCharacters() && !xmlReader.isWhitespace()) {
                                b4s.title = xmlReader.text().toString();
                            }
                            xmlReader.skipCurrentElement();
                        }
                    }
                    tracks.push_back(b4s);
                    xmlReader.skipCurrentElement();
                }
                xmlReader.readNextStartElement();
            }
            xmlReader.readNext();
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

                file.length      = formatLength(0);
                file.rawLength   = 0;
                file.fileSize    = humanizeBytes(0);
                file.rawFileSize = 0;

                AddItemToPlaylist(file);
            }
        }
        else {
            emit log(error, tr("Looks like this B4S file either empty or corrupted. No files were found to parse so cancelling the loading process..."));
            return false;
        }
    }
    else {
        emit log(error, tr("Failed to parse the XML data of \"%1\"...Here's why: %2").arg(FileName, xmlReader.errorString()));
        return false;
    }

    return true;
}
