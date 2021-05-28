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
#include <versions.h>

QString Stats()
{
    uint64_t totalFileSize = 0;
    long totalLength       = 0;

    for (int f = 0; f < Playlist.size(); f++) {
        totalFileSize += Playlist.at(f).rawFileSize;
        totalLength   += Playlist.at(f).rawLength;
    }

    return QString("%1 Item(s) | %2 | %3").arg(
    QString::number(Playlist.size()),
    humanizeBytes(totalFileSize),
    formatLength(totalLength));
}

bool Exporter::formatHTML(QTextStream &stream)
{
    stream << "<!DOCTYPE html>" << NEWLINE << "<html>" << NEWLINE;
    stream << QString("<!-- This is a auto generated code by %1 Version %2").arg(Product::name, Product::version) << NEWLINE;
    stream << QString("---- %1 <%2> / %3 -->").arg(Developer::name, Developer::email, Developer::domain) << NEWLINE;
    stream << TABCHAR << "<head>" << NEWLINE;
    stream << TABCHAR << TABCHAR << "<meta charset='UTF-8'>" << NEWLINE;
    stream << TABCHAR << TABCHAR << QString("<meta name='Generator' content='%1 V%2'>").arg(Product::name, Product::version) << NEWLINE;
    stream << TABCHAR << TABCHAR << QString("<meta name='Author' content='%1'>").arg(Developer::name) << NEWLINE;
    stream << TABCHAR << TABCHAR << "<style type='text/css'>" << NEWLINE;

    stream << TABCHAR << TABCHAR << TABCHAR << "body { background: #38255d; font-family: Arial, Helvetica, sans-serif; font-size: 11pt; }" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << ".name { margin-left: 10px; font-size: 35pt; color: #9681ca; text-align: left; font-weight: 900; }" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << ".name-small { margin-top: -2em; margin-left: 140px; font-size: 22pt; color: #ccc7d8; text-align: left; }" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << ".sum { margin: 15px 0 0 40px; font-size: 10pt; color: #c9b2ce; }" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << "hr { margin-top: 30px; border: 0; background-color: #FFBF00; height: 1px; }" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << ".val { color: #FFBF00; }" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << "span .header { color: #FFBF00; font-size: 14pt; }" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << ".searchInput { width: 200px; height: 17px; border-radius: 10px; border: 3px solid #FFBF00; outline: none; padding: 5px; font-size: 12px; }" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << ".playlistTable { color: #fff; border-collapse: collapse; width: 100%; border: 1px solid #ddd; font-size: 12px; }" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << ".playlistTable th, .playlistTable td { text-align: left; padding: 5px; }" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << ".playlistTable tr { border-bottom: 1px solid #ddd; }" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << ".playlistTable tr.header, .playlistTable tr:hover { color: #000; background-color: #a2ff00; }" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << ".playlistTable tr td a { color: #a2ff00; font-size: 14px; font-weight: bold; }" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << ".playlistTable tr td a:hover { color: #000; }" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << "footer { font-size: 12px; color: #c9b2ce; text-align: center; }" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << "footer a { font-size: 13px; color: #3285a8; }" << NEWLINE;

    stream << TABCHAR << TABCHAR << "</style>" << NEWLINE;

    stream << TABCHAR << TABCHAR << "<script>" << NEWLINE;

    stream << TABCHAR << TABCHAR << TABCHAR << "//Searchs a table element and hide/show results by changing display style property." << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << QString("//By %1 %2 / %3").arg(Developer::name, Developer::email, Developer::domain) << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << "function doSearch() {" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << TABCHAR << "var input, filter, table, tr, td, i, txtValue;" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << TABCHAR << "input  = document.getElementsByClassName(\"searchInput\")[0];" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << TABCHAR << "filter = input.value.toUpperCase();" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << TABCHAR << "table  = document.getElementsByClassName(\"playlistTable\")[0];" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << TABCHAR << "tr     = table.getElementsByTagName(\"tr\");" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << TABCHAR << "for (i = 0; i < tr.length; i++) {" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << TABCHAR << TABCHAR << "td_title  = tr[i].getElementsByTagName(\"td\")[1];" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << TABCHAR << TABCHAR << "if (td_title) {" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << TABCHAR << TABCHAR << TABCHAR << "txtValue = td_title.textContent || td_title.innerText;" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << TABCHAR << TABCHAR << TABCHAR << "if (txtValue.toUpperCase().indexOf(filter) > -1) tr[i].style.display = \"\";" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << TABCHAR << TABCHAR << TABCHAR << "else tr[i].style.display = \"none\";" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << TABCHAR << TABCHAR << "}" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << TABCHAR << "}" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << "}" << NEWLINE;

    stream << TABCHAR << TABCHAR << "</script>" << NEWLINE;
    stream << TABCHAR << TABCHAR << QString("<title>%1 V%2 HTML Generated Playlist</title>").arg(Product::name, Product::version) << NEWLINE;
    stream << TABCHAR << "</head>" << NEWLINE;
    stream << TABCHAR << "<body>" << NEWLINE;

    stream << TABCHAR << TABCHAR << "<div>" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << QString("<div class='name' align='center'><p>%1</p></div>").arg(Product::name) << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << QString("<div class='name-small' align='center'><p>%1</p></div>").arg(PlaylistTitle) << NEWLINE;
    stream << TABCHAR << TABCHAR << "</div>" << NEWLINE;
    stream << TABCHAR << TABCHAR << QString("<hr><div class='sum'>%1</div><blockquote>").arg(Stats()) << NEWLINE;
    stream << TABCHAR << TABCHAR << "<p><input type='text' class='searchInput' onkeyup='doSearch()' placeholder='Search by title...'></p>" << NEWLINE;

    stream << TABCHAR << TABCHAR << "<table class='playlistTable'>" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << "<tr class='header'>" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << TABCHAR << "<th style='width:5%;'></th>" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << TABCHAR << "<th style='width:30%;'>Title</th>" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << TABCHAR << "<th style='width:20%;'>Artist</th>" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << TABCHAR << "<th style='width:30%;'>Album</th>" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << TABCHAR << "<th style='width:7%;'>Length</th>" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << TABCHAR << "<th style='width:7%;'>File Size</th>" << NEWLINE;
    stream << TABCHAR << TABCHAR << TABCHAR << "</tr>" << NEWLINE;

    QStringList failedPathPrefix; QStringList missingFiles;

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

        QString artist = track.artist.isEmpty() ? "Unknown" : track.artist;
        QString title = track.title.isEmpty() ? "Untitled" : track.title;
        QString album = track.album.isEmpty() ? "Unknown" : track.album;

        stream << TABCHAR << TABCHAR << TABCHAR << "<tr>" << NEWLINE;
        stream << TABCHAR << TABCHAR << TABCHAR << TABCHAR << QString("<td><a href='%1'>OPEN</a></td>").arg(suitForXML(filePath)) << NEWLINE;
        stream << TABCHAR << TABCHAR << TABCHAR << TABCHAR << QString("<td>%1</td>").arg(suitForXML(title)) << NEWLINE;
        stream << TABCHAR << TABCHAR << TABCHAR << TABCHAR << QString("<td>%1</td>").arg(suitForXML(artist)) << NEWLINE;
        stream << TABCHAR << TABCHAR << TABCHAR << TABCHAR << QString("<td>%1</td>").arg(suitForXML(album)) << NEWLINE;
        stream << TABCHAR << TABCHAR << TABCHAR << TABCHAR << QString("<td>%1</td>").arg(track.length) << NEWLINE;
        stream << TABCHAR << TABCHAR << TABCHAR << TABCHAR << QString("<td>%1</td>").arg(track.fileSize) << NEWLINE;
        stream << TABCHAR << TABCHAR << TABCHAR << "</tr>" << NEWLINE;
    }


    stream << TABCHAR << TABCHAR << "</table></blockquote>" << NEWLINE;
    stream << TABCHAR << "</body>" << NEWLINE;
    stream << TABCHAR << "<footer>" << NEWLINE;
    stream << TABCHAR << TABCHAR << QString("This is a auto generated HTML document by %1 Version %2"
    "<br>2019 &#169; %3 &#60;%4&#62;<br><a href='https://%5' target='_blank'>%5</a>").arg(
    Product::name, Product::version, Developer::name, Developer::email, Developer::domain) << NEWLINE;
    stream << TABCHAR << "</footer>" << NEWLINE;
    stream << "</html>";

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
