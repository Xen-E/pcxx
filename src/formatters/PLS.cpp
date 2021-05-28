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


#include <exporter.h>

bool Exporter::formatPLS(QTextStream &stream)
{
    QStringList failedPathPrefix; QStringList missingFiles;

    stream << "[playlist]" + NEWLINE + NEWLINE;

    emit setPBMax(Playlist.size());

    for (auto &track : Playlist) {
        int index((&track - Playlist.begin()) + 1);

        if (Performance > 0) {
            QThread::msleep(Performance);
        }

        emit setPB(index);

        if (_CANCEL_EXPORTING) {
            emit log(warn, tr("Cancel signal has been made, Cancelling exportation process..."));
            break;
        }

        QString filePath(track.fullPath);

        if (PathPrefix) {
            const int f_pos(filePath.indexOf(PathPrefixFind, 0, caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive));
            if (f_pos != -1) {
                filePath.replace(f_pos, PathPrefixFind.length(), PathPrefixReplace);
            }
            else {
                if (PathPrefixFailExclude) {
                    failedPathPrefix.push_back(filePath);
                    continue;
                }
            }
        }

        if (CheckFileExistence) {
            if (!QFile::exists(filePath)) {
                missingFiles.push_back(filePath);
                continue;
            }
        }

        QString title = track.title.isEmpty() ? "Untitled" : track.title;

        const QString fileLine(QString("File%1=%2").arg(QString::number(index), filePath));
        const QString titleLine(QString("Title%1=%2").arg(QString::number(index), title));
        const QString lengthLine(QString("Length%1=%2").arg(QString::number(index), QString::number(track.rawLength)));

        stream << fileLine << NEWLINE;
        stream << titleLine << NEWLINE;
        stream << lengthLine << NEWLINE << NEWLINE;
    }

    stream << QString("NumberOfEntries=%1").arg(Playlist.size()) << NEWLINE;
    stream << "Version=2";

    const int failedFiles(failedPathPrefix.size() + missingFiles.size());

    if (!_CANCEL_EXPORTING) {
        if (failedFiles == 0) {
            emit log(done, tr("All files formatted with no issues and ready to be exported."));
            return true;
        }
        else if (failedFiles > 0 && failedFiles < Playlist.size()) {
            emit log(error, tr("Some files had no issues but these failed to pass the tests and were excluded:"));

            if (failedPathPrefix.size() > 0) {
                emit log(error, tr("================= Path Prefix Failure (%1): =================").arg(QString::number(failedPathPrefix.size())));
                for (auto &failedPathPrefixFile : failedPathPrefix) {
                    emit log(warn, tr("File -> \"%1\".").arg(failedPathPrefixFile));
                }
            }
            if (missingFiles.size() > 0) {
                emit log(error, tr("==================== MISSING FILES (%1): ====================").arg(QString::number(missingFiles.size())));
                for (auto &missingFile : missingFiles) {
                    emit log(warn, tr("File -> \"%1\".").arg(missingFile));
                }
            }

            emit log(info, tr("Exportation process will continue anyway..."));
            return true;
        }
        else if (failedFiles == Playlist.size()) {
            emit log(error, tr("All files failed to pass the tests and were excluded from exportation process."));
            emit log(error, tr("This behavior is affected by \"Path Prefix\" and \"Checking file existence\" options."));
            emit log(error, tr("There's 0 files to export so cancelling..."));
            return false;
        }
    }
    else {
        emit log(warn, tr("Exportation process was CANCELLED by user."));
        return false;
    }

    return false;
}
