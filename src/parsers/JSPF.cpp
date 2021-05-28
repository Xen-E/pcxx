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
#include <misc.h>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <QVariantMap>

bool Parser::parseJSPF(QTextStream &stream)
{
    QJsonParseError JPE;
    QJsonDocument doc = QJsonDocument::fromJson(stream.readAll().toUtf8(), &JPE);

    if (doc.isEmpty()) {
        emit log(error, tr("JSON file \"%1\" is empty. Cancelling parsing process...").arg(FileName));
        return false;
    }
    if (doc.isNull()) {
        emit log(error, tr("Failed to parse JSON file \"%1\", Here's why: %2").arg(FileName, JPE.errorString()));
        return false;
    }

    QJsonObject jsonObj = doc.object();
    QVariantMap mainMap = jsonObj.toVariantMap();
    QVariantMap dataMap = mainMap["playlist"].toMap();
    QJsonArray tracks = dataMap["track"].toJsonArray();

    if (tracks.size() > 0) {
        struct JSPF {
            QString creator, title, album, location;
            unsigned long duration;
        };
        QVector<JSPF> parsedTracks;

        foreach (const QJsonValue &track, tracks) {
            const QString location(track["location"].toArray().at(0).toString());
            if (location.isEmpty())
                continue;

            JSPF jspf;
            jspf.creator = track["creator"].toString();
            jspf.title = track["title"].toString();
            jspf.album = track["album"].toString();
            jspf.location = location;

            const QString duration(track["duration"].toString());
            if (!duration.isEmpty()) jspf.duration = duration.toULong();
            else jspf.duration = 0;

            parsedTracks.push_back(jspf);
        }

        if (parsedTracks.size() > 0) {
            emit setPBMax(parsedTracks.size());

            for (auto &track : parsedTracks) {
                int index((&track - parsedTracks.begin()) + 1);

                if (_CANCEL_LOADING) {
                    emit log(warn, tr("Cancel signal has been made, Cancelling loading process..."));
                    return false;
                }

                if (Performance > 0)
                    QThread::msleep(Performance);

                emit setPB(index);

                File file;
                file.ID = generateID();

                file.name     = BaseName(track.location);
                file.fullPath = track.location;

                file.title  = track.title;
                file.artist = track.creator;
                file.album  = track.album;

                file.length      = formatLength(track.duration);
                file.rawLength   = track.duration;
                file.fileSize    = humanizeBytes(0);
                file.rawFileSize = 0;

                AddItemToPlaylist(file);
            }
        }
        else {
            emit log(error, tr("Failed to load tracks from \"%1\", Looks like the file is NOT empty, there's tracks in it"
            " But none of it have any locations (file paths).").arg(FileName));
            return false;
        }
    }
    else {
        emit log(error, tr("File \"%1\" is empty (kind of), Couldn't find any tracks in it.").arg(FileName));
        return false;
    }

    return true;
}
