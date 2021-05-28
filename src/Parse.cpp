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

bool _PLAYLIST_BUSY_LOADING;

void Parser::Parse()
{
    if (FileName.isEmpty()) {
        emit log(error, tr("No playlist file provided! Cancelling..."));
        return;
    }

    emit disableUI(true); _PLAYLIST_BUSY_LOADING = true;

    bool failed = false;
    QElapsedTimer timer; timer.start();

    QString performanceUI;
    if (Performance == 0) performanceUI = "Maximum";
    else if (Performance > 0 && Performance <= 25) performanceUI = "Medium";
    else if (Performance > 25 && Performance == 49) performanceUI = "Slow";
    else if (Performance == 50) performanceUI = "Minimum";

    emit log(info, tr("Performance is set to: %1 (%2)").arg(performanceUI, QString::number(Performance)));

    if (Type != Format::FPL) {
        emit log(info, tr("Decoding in: \"%1\"").arg(Codec));
        emit log(info, tr("Trying to open file \"%1\"...").arg(FileName));

        QFile fs(FileName);

        if (fs.open(QIODevice::ReadOnly | QIODevice::Text)) {
            emit log(info, tr("File opened successfully, reading data..."));

            QTextStream stream (&fs);
            const QString c_name(stream.codec()->name().toStdString().c_str());
            emit log(info, tr("Looks like this file is encoded with: %1").arg(c_name));
            stream.setCodec(Codec.toStdString().c_str());

            switch (Type) {
            case Format::AIMPPL:
                emit log(info, tr("Parsing \"%1\"... (AIMPPL)").arg(FileName));
                failed = !parseAIMPPL(stream);
                break;
            case Format::ASX:
                emit log(info, tr("Parsing \"%1\"... (Advanced Stream Redirector)").arg(FileName));
                failed = !parseASX(fs);
                break;
            case Format::B4S:
                emit log(info, tr("Parsing \"%1\"... (B4S/old Winamp)").arg(FileName));
                failed = !parseB4S(fs);
                break;
            case Format::M3U:
                emit log(info, tr("Parsing \"%1\"... (M3U/M3U8, MP3 URL)").arg(FileName));
                failed = !parseM3U(stream);
                break;
            case Format::MPCPL:
                emit log(info, tr("Parsing \"%1\"... (Media Player Classic)").arg(FileName));
                failed = !parseMPCPL(stream);
                break;
            case Format::PLS:
                emit log(info, tr("Parsing \"%1\"... (PLS, museArc/SHOUTcast)").arg(FileName));
                failed = !parsePLS(stream);
                break;
            case Format::WPL:
                emit log(info, tr("Parsing \"%1\"... (Microsoft Windows Media Player)").arg(FileName));
                failed = !parseWPL(fs);
                break;
            case Format::XSPF:
                emit log(info, tr("Parsing \"%1\"... (XML Shareable Playlist Format)").arg(FileName));
                failed = !parseXSPF(fs);
                break;
            case Format::JSPF:
                emit log(info, tr("Parsing \"%1\"... (JSON Shareable Playlist Format)").arg(FileName));
                failed = !parseJSPF(stream);
                break;
            case Format::TXT:
                emit log(info, tr("Parsing \"%1\"... (Text Document)").arg(FileName));
                failed = !parseTXT(stream);
                break;

            case Format::FPL:
                break;
            case Format::HTML:
                break;
            }

            fs.close();
        }
        else {
            emit log(error, tr("Failed to open file \"%1\", Here's why: %2").arg(FileName, fs.errorString()));
            emit ShowMessageBox(error, tr("Loading process failed"), tr("Failed load the following playlist file:%1%2%1%1"
            "Please check logs for more details.").arg(NEWLINE, FileName));
        }
    }
    else {
        emit log(warn, tr("Current parsing format is FPL (foobar2000) playlist file, "
        "Which means character decoding will NOT be performed since it's a binary file."));
        emit log(info, tr("Parsing \"%1\"... (foobar2000)").arg(FileName));
        failed = !parseFPL();
    }

    if (!failed) {
        emit log(done, tr("Finished parsing file \"%1\".").arg(FileName));
        emit log(info, tr("The whole process took: %1.").arg(formatTimer(timer.elapsed())));

        if (ShowMsgWhenDone) {
            emit ShowMessageBox(info, tr("Loading process finished"),
            tr("All data/files from this following file were parsed successfully:%1%2%1%1"
            "You can turn this notification off from settings.").arg(NEWLINE, FileName));
        }
    }
    else {
        if (_CANCEL_LOADING) {
            emit log(warn, tr("Not all files were parsed/loaded due to cancellation."));
        }
        else {
            emit log(error, tr("Couldn't parse file \"%1\".").arg(FileName));
            emit ShowMessageBox(error, tr("Parsing failed"), tr("Looks like loading process was stopped due to failure "
            "NOT cancellation, Please check logs for more details."));
        }
    }

    emit disableUI(false); _PLAYLIST_BUSY_LOADING = false; _UNEXPORTED = true;

    emit UpdateStats();
    emit UpdateFilesList();
}
