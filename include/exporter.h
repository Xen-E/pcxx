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


#ifndef EXPORTER_H
#define EXPORTER_H

#include <types.h>
#include <misc.h>

#include <QTextStream>
#include <QDir>
#include <QThread>
#include <QElapsedTimer>

class Exporter : public QObject
{
    Q_OBJECT

public:
    Format format;
    QString SavePath, PlaylistTitle;

    bool PathPrefix; QString PathPrefixFind, PathPrefixReplace; bool caseSensitive;
    QString Codec;
    int Performance;

    bool PathPrefixFailExclude,
    CheckFileExistence,
    ShowInFolderWhenDone,
    ShowMsgWhenDone,
    ExecuteCMD;
    QString CMD;

signals:
    void log(levelType type, const QString &log);
    void ShowMessageBox(levelType type, const QString &title, const QString &msg);
    void setPBMax(const int value);
    void setPB(const int value);
    void disableUI(const bool really);

private:
    bool formatM3U(QTextStream &stream);
    bool formatPLS(QTextStream &stream);
    bool formatMPCPL(QTextStream &stream);
    bool formatXSPF(QTextStream &stream);
    bool formatJSPF(QTextStream &stream);
    bool formatWPL(QTextStream &stream);
    bool formatASX(QTextStream &stream);
    bool formatB4S(QTextStream &stream);
    bool formatHTML(QTextStream &stream);
    bool formatTXT(QTextStream &stream);

    /**
     * @brief Takes a QString and make it suitable for XML formatting (Special Characters)
     * @param str QString to be parsed
     * @return QString contains escpaed characters
     */
    const QString suitForXML(QString &str) {
        QString escapedStr;
        for(QChar &c : str) {
            if (c == '&') escapedStr += "&amp;"; //Ampersand sign
            else if (c == '\'') escapedStr += "&apos;"; //Single quote
            else if (c == '"') escapedStr += "&quot;"; //Double quote
            else if (c == '<') escapedStr += "&lt;"; //Lesser than
            else if (c == '>') escapedStr += "&gt;"; //Greater than
            else escapedStr += c;
        }
        return escapedStr;
    }

public slots:
    void Export();

public:
    virtual ~Exporter() { delete this; }

};

extern bool _CANCEL_EXPORTING;
extern bool _UNEXPORTED;

#endif // EXPORTER_H
