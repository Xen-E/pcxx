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


#include <misc.h>

/**
 * @brief Checks if a file path already exists in a QStringList
 * @param filePath File path to look for
 * @param excludedList The QStringList
 * @return true if exists, false otherwise
 */
bool existInExcludedList(const QString &filePath, const QStringList &excludedList)
{
    foreach (const QString &path, excludedList) {
        if (filePath == path)
            return true;
    }
    return false;
}
/**
 * @brief Filters a directory by list of extensions
 * @param extensions QStringList contains the extensions separated by a space
 * @param files QStringList contains the files to be filtered
 * @param caseSensitive Does the extension matching need to be case-sensitive? (mp3=Mp3)
 * @return QStringList Filtered files list
 */
QStringList filterDir(const QString &extensions, const QStringList &files, bool caseSensitive)
{
    const QStringList filter(extensions.split(" "));
    QStringList excludedFiles;

    foreach (const QString &ext, filter) {
        foreach (const QString &path, files) {

            QString pathExt(path.right(ext.length()));
            if (!caseSensitive) pathExt = pathExt.toLower();

            if (pathExt == ext)
                excludedFiles.push_back(path);
        }
    }

    if (excludedFiles.size() == 0)
        return files;

    QStringList filteredFiles;
    foreach (const QString &path, files) {
        if (!existInExcludedList(path, excludedFiles))
            filteredFiles.push_back(path);
    }
    return filteredFiles;
}
