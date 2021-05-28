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
 * @brief Checks if a file path exist in playlist vector
 * @param filePath File path to match
 * @return true if exists, false otherwise
 */
bool ExistsInPlaylist(const QString &filePath)
{
    for (auto &track : Playlist) {
        if (track.fullPath == filePath)
            return true;
    }
    return false;
}

/**
 * @brief Handles items appending to Playlist vector
 * @param file strcut to be added
 */
void AddItemToPlaylist(File &file)
{
    if (!ExistsInPlaylist(file.fullPath))
        Playlist.push_back(file);
}

/**
 * @brief Deletes an item from playlist vector
 * @param ID Identifier of the unwanted item
 * @return true if successful, false otherwise
 */
bool DeleteItemInPlaylist(const unsigned long &ID)
{
    for (auto &track : Playlist) {
        if (track.ID == ID) {
            Playlist.erase(Playlist.begin()+(&track-Playlist.begin()));
            return true;
        }
    }
    return false;
}

/**
 * @brief Gets the index of an item by file path
 * @param filePath The file path to look for
 * @return Index of the item in playlist, -1 if not found
 */
int GetItemIndexByFilePath(const QString &filePath)
{
    for (auto &track : Playlist) {
        if (track.fullPath == filePath)
            return (&track-Playlist.begin());
    }
    return -1;
}
