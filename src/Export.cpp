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

bool _PLAYLIST_BUSY_EXPORTING;

void Exporter::Export()
{
    if (SavePath.isEmpty()) {
        emit log(error, tr("No save path provided! Cancelling..."));
        return;
    }

    emit disableUI(true); _PLAYLIST_BUSY_EXPORTING = true;

    QString performanceUI;
    if (Performance == 0) performanceUI = "Maximum";
    else if (Performance > 0 && Performance <= 25) performanceUI = "Medium";
    else if (Performance > 25 && Performance == 49) performanceUI = "Slow";
    else if (Performance == 50) performanceUI = "Minimum";

    emit log(info, tr("Performance is set to: %1 (%2)").arg(performanceUI, QString::number(Performance)));
    emit log(info, tr("Is Path Prefix enabled?: %1").arg(PathPrefix ? "YES" : "NO"));
    emit log(info, tr("Path Prefix, Find: \"%1\"").arg(PathPrefixFind));
    emit log(info, tr("Path Prefix, Replace: \"%1\"").arg(PathPrefixReplace));
    emit log(info, tr("Check for file existence: %1").arg(CheckFileExistence ? "YES" : "NO"));
    emit log(info, tr("Playlist Title: \"%1\"").arg(PlaylistTitle));
    emit log(info, tr("Encoding in: \"%1\"").arg(Codec));

    emit log(info, tr("%1OK. Creating a file to write data into; \"%2\"...").arg(NEWLINE, SavePath));
    QFile fs(SavePath);

    if (fs.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit log(done, tr("File created successfully, formatting data..."));

        QTextStream stream (&fs);
        stream.setCodec(Codec.toStdString().c_str());

        bool failed = false;
        QElapsedTimer timer; timer.start();

        switch (format) {
        case Format::M3U:
            emit log(info, tr("Using \"M3U/M3U8 (MP3 URL)\" format..."));
            failed = !formatM3U(stream);
            break;
        case Format::PLS:
            emit log(info, tr("Using \"PLS (SHOUTcast/museArc)\" format..."));
            failed = !formatPLS(stream);
            break;
        case Format::XSPF:
            emit log(info, tr("Using \"XML Shareable Playlist\" format..."));
            failed = !formatXSPF(stream);
            break;
        case Format::JSPF:
            emit log(info, tr("Using \"JSON Shareable Playlist\" format..."));
            failed = !formatJSPF(stream);
            break;
        case Format::WPL:
            emit log(info, tr("Using \"Microsoft Windows Media Player\" format..."));
            failed = !formatWPL(stream);
            break;
        case Format::ASX:
            emit log(info, tr("Using \"Advanced Stream Redirector (ASX)\" format..."));
            failed = !formatASX(stream);
            break;
        case Format::MPCPL:
            emit log(info, tr("Using \"Media Player Classic\" format..."));
            failed = !formatMPCPL(stream);
            break;
        case Format::VDJ:
            emit log(info, tr("Using \"VirtualDJ playlists\" format..."));
            failed = !formatVDJ(stream);
            break;
        case Format::B4S:
            emit log(info, tr("Using \"B4S (old Winamp)\" format..."));
            failed = !formatB4S(stream);
            break;
        case Format::HTML:
            emit log(info, tr("Using \"HyperText Markup Language (HTML)\" format..."));
            failed = !formatHTML(stream);
            break;
        case Format::TXT:
            emit log(info, tr("Using \"Text Document\" format..."));
            failed = !formatTXT(stream);
            break;

        case Format::AIMPPL:
            break;
        case Format::FPL:
            break;
        }

        if (!failed) {
            emit log(done, tr("Done! Data was formatted and written into the file successfully."));
            emit log(info, tr("The whole process took: %1.").arg(formatTimer(timer.elapsed())));

            if (ShowMsgWhenDone) {
                emit ShowMessageBox(info, tr("Exportation process finished"),
                tr("Playlist created successfully and ready to be used. Check:%1\"%2\"").arg(NEWLINE, SavePath));
            }
            if (ShowInFolderWhenDone) {
                showInFolder(SavePath);
            }
            if (ExecuteCMD) {
                if (system(NULL) != 0) {
                    const QString pl("{playlist}");
                    #if defined (__WIN32__)
                    SavePath.replace('/', QDir::separator());
                    #endif
                    int f_pos(CMD.indexOf(pl, 0, Qt::CaseSensitive));
                    if (f_pos != -1)
                        CMD.replace(f_pos, pl.length(), SavePath);

                    int res = system(CMD.toStdString().c_str());
                    if (res < 0)
                        emit log(warn, tr("Command \"%1\" failed to execute, Command processor returned: %2").arg(CMD, QString::number(res)));
                }
                else {
                    emit log(warn, tr("Unable to execute commands, Looks like no command processor is available on this OS."));
                    emit ShowMessageBox(warn, tr("No command processor is available"), tr("Couldn't execute any commands, Please check logs for more info."));
                }
            }
            fs.close();
        }
        else {
            if (!fs.remove())
                emit log (error, tr("Failed to remove \"%1\", Here's why: %2").arg(SavePath, fs.errorString()));
            emit log(error, tr("Exportation process failed. Check logs above."));
            emit ShowMessageBox(error, tr("Exportation process failed"),
            tr("Not everything went well, Looks like the exportation process failed. "
            "Please check logs for more details."));
        }
    }
    else {
        emit log(error, tr("Failed to create a new file, Here's why: %1").arg(fs.errorString()));
        emit ShowMessageBox(error, tr("Exportation process failed"), tr("Failed to produce a new playlist, Please check logs for more details."));
    }

    emit disableUI(false); _PLAYLIST_BUSY_EXPORTING = false; _UNEXPORTED = false;
}
