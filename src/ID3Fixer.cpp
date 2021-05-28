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


#include <id3Fixer.h>

bool _FILES_ARE_GETTING_FIXED;

void ID3Fixer::Fix()
{
    emit log(info, tr("Trying to fix ID3 data of %1 file(s)...").arg(QString::number(Playlist.size())));

    emit disableUI(true); _FILES_ARE_GETTING_FIXED = true;

    emit setPBMax(Playlist.size());

    QStringList failedFiles;

    QElapsedTimer timer; timer.start();

    for (auto &track : Playlist) {
        emit setPB((&track - Playlist.begin()));

        if (_CANCEL_LOADING) {
            emit log(warn, tr("Cancel signal has been made, Cancelling ID3 fixing process..."));
            break;
        }

        TagLib::FileRef f(
        #if defined(_WIN32)
        //It's important to use wide characters (wchar_t) for windows platforms
        //or unicode will not work.
        track.fullPath.toStdWString().c_str()
        #else
        //*nix systems support unicode by default...so standard string is fine.
        track.fullPath.toStdString().c_str()
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
        else {
            failedFiles.push_back(track.fullPath);
            emit log(error, tr("Failed to get ID3 data from: \"%1\".").arg(track.fullPath));
        }

        if (!f.isNull() && f.audioProperties()->lengthInSeconds()) {
            TagLib::AudioProperties *AP = f.audioProperties();
            rawLength = AP->lengthInSeconds();
        }
        else {
            if (!failedFiles.contains(track.fullPath))
                failedFiles.push_back(track.fullPath);
            emit log(error, tr("Failed to read audio properties (Length) from: \"%1\".").arg(track.fullPath));
        }

        track.title  = title;
        track.artist = artist;
        track.album  = album;
        track.length      = formatLength(rawLength);
        track.rawLength   = rawLength;

        const uint64_t f_size(fileSizeBytes(track.fullPath));
        track.fileSize    = humanizeBytes(f_size);
    }

    if (!_CANCEL_LOADING) {
        if (failedFiles.size() == 0) {
            emit log(done, tr("All files were processed with no errors."));
            emit ShowMessageBox(info, tr("ID3 fixed"), tr("The ID3 data of all files were fixed and updated."
            " If some are missing then the issue is the file itself, Use an ID3 tag editor."));
        }
        else if (failedFiles.size() > 0 && failedFiles.size() < Playlist.size()) {
            emit log(error, tr("Failed to read the ID3 data of the following files:"));
            for (auto &failedFile : failedFiles) {
                emit log(warn, QString("FILE => \"%1\"").arg(failedFile));
            }
            emit ShowMessageBox(warn, tr("ID3 failure"), tr("Not all files were processed, Failed to read the ID3 data of SOME files. "
            "Please check logs for more details."));
        }
        else if (failedFiles.size() == Playlist.size()) {
            emit log(error, tr("Couldn't process any file! Weird."));
            emit ShowMessageBox(error, tr("ID3 failure"), tr("Failed to read the ID3 data of ALL files, Please check logs for more details."));
        }

        emit log(info, tr("ID3 fixing process finished. It took %1").arg(formatTimer(timer.elapsed())));
    }
    else { emit log(warn, tr("Not all files were processed due to user cancellation.")); }

    emit disableUI(false); _FILES_ARE_GETTING_FIXED = false;

    emit UpdateStats();
    emit UpdateFilesList();
}
