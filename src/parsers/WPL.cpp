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

bool Parser::parseWPL(QFile &fileStream)
{
    QStringList srcMedia;

    QXmlStreamReader xmlReader(&fileStream);
    while (!xmlReader.atEnd() && !xmlReader.hasError()) {

        xmlReader.readNext();

        if (xmlReader.isStartDocument()) {continue;}
        if (xmlReader.isEndDocument()) {break;}

        if (xmlReader.name().toString() == "smil") { // root

            xmlReader.readNextStartElement(); // head
            xmlReader.skipCurrentElement(); // body
            xmlReader.readNextStartElement(); // seq
            while (xmlReader.readNextStartElement()) { // media elements
                while (xmlReader.readNextStartElement()) {
                    if (xmlReader.name().toString() == "media") {
                        QXmlStreamAttributes attrs(xmlReader.attributes());
                        if (attrs.hasAttribute("src")) {
                            srcMedia.push_back(attrs.value("src").toString());
                        }
                    }
                    xmlReader.skipCurrentElement();
                }
            }
            xmlReader.readNext();
        }
    }

    if (!xmlReader.hasError()) {
        emit log(info, tr("Parsing was successful, Now adding data to the list..."));

        if (srcMedia.size() > 0) {
            emit setPBMax(srcMedia.size());

            for (int f = 0; f < srcMedia.size(); f++) {
                if (_CANCEL_LOADING) {
                    emit log(warn, tr("Cancel signal has been made, Cancelling loading process..."));
                    return false;
                }

                if (Performance > 0)
                    QThread::msleep(Performance);

                emit setPB(f);

                emit log(info, tr("Working on \"%1\"...").arg(srcMedia.at(f)));

                File file;
                file.ID = generateID ();

                file.name     = BaseName(srcMedia.at(f));
                file.fullPath = srcMedia.at(f);

                file.title  = "";
                file.artist = "";
                file.album  = "";

                file.length      = formatLength(0);
                file.rawLength   = 0;
                file.fileSize    = humanizeBytes(0);
                file.rawFileSize = 0;

                AddItemToPlaylist(file);
            }
        }
        else {
            emit log(error, tr("Hmmm, looks like this WPL file either empty or corrupted. No files were found so cancelling the loading process."));
            return false;
        }
    }
    else {
        emit log(error, tr("Failed to parse the XML data of \"%1\"...Here's why: %2").arg(FileName, xmlReader.errorString()));
        return false;
    }

    return true;
}
