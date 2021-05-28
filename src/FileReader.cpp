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


#include <fileReader.h>

bool _FILES_BUSY_LOADING;

void FileReader::Read()
{
    emit disableUI(true); _FILES_BUSY_LOADING = true;

    emit setPBMax(files.size());

    QElapsedTimer timer; timer.start();
    int PBVal = 0;

    for (auto &filePath : files) {
        if (!dir.isEmpty())
            filePath = QString("%1%2%3").arg(dir, "/", filePath);

        if (_CANCEL_LOADING) {
            emit log(warn, tr("Cancel signal has been made, Cancelling loading process..."));
            break;
        }

        if (Performance > 0)
            QThread::msleep(Performance);

        emit setPB(PBVal); PBVal++;

        TagLib::FileRef f(
        #if defined(_WIN32)
        //It's important to use wide characters (wchar_t) for windows platforms
        //or unicode will not work.
        filePath.toStdWString().c_str()
        #else
        //*nix systems support unicode by default...so standard string is fine.
        filePath.toStdString().c_str()
        #endif
        , true, TagLib::AudioProperties::ReadStyle::Fast);

        QString title, artist, album;
        unsigned long rawLength = 0;

        if (!f.isNull() && f.tag()) {
            TagLib::Tag *tag = f.tag();
            title  = tag->title().isEmpty()  ? "Untitled" : QString::fromUtf8(tag->title().to8Bit(true).c_str());
            artist = tag->artist().isEmpty() ? "Unknown"  : QString::fromUtf8(tag->artist().to8Bit(true).c_str());
            album  = tag->album().isEmpty()  ? "Unknown"  : QString::fromUtf8(tag->album().to8Bit(true).c_str());
        }
        else { emit log(error, tr("Failed to get ID3 data from: \"%1\".").arg(filePath)); }

        if (!f.isNull() && f.audioProperties()->lengthInSeconds()) {
            TagLib::AudioProperties *AP = f.audioProperties();
            rawLength = AP->lengthInSeconds();
        }
        else { emit log(error, tr("Failed to read audio properties (Length) from: \"%1\".").arg(filePath)); }

        File file;
        file.ID = generateID();

        file.name     = BaseName(filePath);
        file.fullPath = filePath;

        file.title  = title;
        file.artist = artist;
        file.album  = album;

        file.length      = formatLength(rawLength);
        file.rawLength   = rawLength;

        const uint64_t f_size(fileSizeBytes(filePath));
        file.fileSize    = humanizeBytes(f_size);
        file.rawFileSize = f_size;

        AddItemToPlaylist(file);

    }

    if (_CANCEL_LOADING) {
        emit log(warn, tr("Not all files were loaded due to cancellation by user."));
    }
    else {
        emit log(done, tr("Files loading process finished, %1 files has been processed.").arg(files.size()));
        if (ShowMsgWhenDone) {
            emit ShowMessageBox(info, tr("Loading process finished"),
            tr("All files were processed and loaded successfully.%1%1"
            "You can turn this notification off from settings.").arg(NEWLINE));
        }
    }
    emit log(info, tr("The whole process took: %1.").arg(formatTimer(timer.elapsed())));

    emit disableUI(false); _FILES_BUSY_LOADING = false; _UNEXPORTED = true;

    emit UpdateStats();
    emit UpdateFilesList();
}
