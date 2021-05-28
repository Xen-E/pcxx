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

#include <random>
#include <algorithm>

using namespace std;

/**
 * @brief Checks if the provided ID exist in playlist vector
 * @param ID Identifier to match
 * @return true if exists, false otherwise
 */
bool ID_Exist(unsigned long &ID)
{
    for (auto &track : Playlist) {
        if (track.ID == ID)
            return true;
    }
	return false;
}

/**
 * @brief Generates a random 10 digit identifier, If already exists then it will generate again.
 * @return 10 digit identifier
 */
unsigned long generateID()
{
    static QString digits = "0123456789";
    static mt19937 rng (random_device{}());

	again:
    shuffle(digits.begin(), digits.end(), rng);
    unsigned long ID(stoull(digits.toStdString()));

    if (ID_Exist(ID))
		goto again;

	return ID;
}
