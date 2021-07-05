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


#ifndef VERSIONS_H
#define VERSIONS_H

#include <QString>

namespace Architecture {
    const bool i686 = sizeof(void*) != 8 ? true : false;
    const QString humanReadable = i686 ? "32bit" : "64bit";
}

namespace Developer {
    const QString domain = "xen-e.github.io";
    const QString name   = "Xen";
    const QString email  = "xen-dev@pm.me";
}

namespace Product {
    const QString name       = "Playlist Converter++";
    const QString shortName  = "PC++";
    const unsigned int major = 1;
    const unsigned int minor = 1;
    const QString version    = QString("%1.%2").arg(QString::number(major), QString::number(minor));
}

namespace Libraries {
    const QString qtVersion = QString("%1.%2.%3").arg(
    QString::number(QT_VERSION_MAJOR),
    QString::number(QT_VERSION_MINOR),
    QString::number(QT_VERSION_PATCH));

    const QString taglibVersion = "1.11.1";
}

namespace usedCompiler {
    #if defined (__llvm__) || defined (__clang__)
        const QString name    = "clang";
        const QString version = QString("%1.%2.%3").arg(
        QString::number(__clang_major__),
        QString::number(__clang_minor__),
        QString::number(__clang_patchlevel__));
    #elif __GNUC__
        const QString name    = "GCC";
        const QString version = QString("%1.%2.%3").arg(
        QString::number(__GNUC__),
        QString::number(__GNUC_MINOR__),
        QString::number(__GNUC_PATCHLEVEL__));
    #elif _MSC_VER
        const QString name    = "MSVC";
        const QString version = QString("%1").arg(QString::number(_MSC_VER));
    #elif __MINGW32__
        const QString name    = "MinGW32";
        const QString version = QString("%1.%2").arg(
        QString::number(__MINGW32_MAJOR_VERSION),
        QString::number(__MINGW32_MINOR_VERSION));
    #elif __MINGW64__
        const QString name    = "MinGW64";
        const QString version = QString("%1.%2").arg(
        QString::number(__MINGW64_VERSION_MAJOR),
        QString::number(__MINGW64_VERSION_MINOR));
    #elif
        const QString name    = "Unknown compiler";
        const QString version = "N/A";
    #endif
}


#endif // VERSIONS_H
