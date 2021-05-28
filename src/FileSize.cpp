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
 * @brief Reads file size in bytes
 * @param filePath Path to the file
 * @return File size in bytes
 */
uint64_t fileSizeBytes(const QString &filePath)
{
    QFileInfo fi(filePath);
    return fi.size();
}

/**
 * @brief Makes file size human-readable
 * @param bytes File bytes
 * @return QString suitable for user reading
 */
const QString humanizeBytes(uint64_t bytes)
{
	const char * suffix[] = { "B", "KB", "MB", "GB", "TB" };
    char length = sizeof(suffix) / sizeof(suffix[0]);

	int i = 0;
	double dblBytes = bytes;

    if (bytes > 1024) {
        for  (i = 0; (bytes / 1024) > 0 && i < length-1; i++, bytes /= 1024)
			dblBytes = bytes / 1024.0;
	}

	static char output[200];
    sprintf (output, "%.02lf %s", dblBytes, suffix[i]);
    
	return output;
}
