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


#ifndef PARSER_H
#define PARSER_H

#include <types.h>
#include <misc.h>

#include <QFile>
#include <QThread>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QTextCodec>
#include <QElapsedTimer>

class Parser : public QObject
{
    Q_OBJECT

public:
    QString FileName, Codec;
    Format Type;
    int Performance;
    bool ShowMsgWhenDone;

signals:
    void log(levelType type, const QString &log);
    void ShowMessageBox(levelType type, const QString &title, const QString &msg);
    void setPBMax(const int value);
    void setPB(const int value);
    void disableUI(const bool really);
    void UpdateStats();
    void UpdateFilesList();

private:
    bool parseM3U(QTextStream &stream);
    bool parsePLS(QTextStream &stream);
    bool parseMPCPL(QTextStream &stream);
    bool parseAIMPPL(QTextStream &stream);
    bool parseTXT(QTextStream &stream);
    bool parseXSPF(QFile &fileStream);
    bool parseITUNES(QFile &fileStream);
    bool parseJSPF(QTextStream &stream);
    bool parseWPL(QFile &fileStream);
    bool parseASX(QFile &fileStream);
    bool parseB4S(QFile &fileStream);
    bool parseFPL();

    inline const QString trimSpaces(QString str)
    {
        if (str.startsWith(" "))
            str = str.right(str.length()-1);
        if (str.endsWith(" "))
            str = str.left(str.length()-1);

        return str;
    }

public slots:
    void Parse();

public:
    virtual ~Parser() { delete this; }
};

extern bool _CANCEL_LOADING;
extern bool _UNEXPORTED;

#endif // PARSER_H
