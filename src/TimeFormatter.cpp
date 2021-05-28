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

#include <sstream>
#include <iomanip>

using namespace std;

/**
 * @brief Makes timer elapsed time human-readable
 * @param time Time in milliseconds
 * @return QString suitable for user reading
 */
const QString formatTimer(const qint64 &time)
{
    double calcET; stringstream calcSS;

    if (time < 1000) {
        calcSS << fixed << setprecision(2) << time << " millisecond(s)";
    }
    else if (time > 999 && time < 60000) {
        calcET = time / 1000.0;
        calcSS << fixed << setprecision(2) << calcET << " second(s)";
    }
    else if (time > 59999) {
        calcET = (time / 1000.0) / 60.0;
        calcSS << fixed << setprecision(2) << calcET << " minute(s)";
    }

    return QString::fromStdString(calcSS.str());
}

/**
 * @brief Makes track length/duration human-readable
 * @param length Length/Duration in seconds
 * @return QString suitable for user reading
 */
const QString formatLength(unsigned long length)
{
    if (length < 1)
        return "N/A";

    long day = length / (24 * 3600);

    length = length % (24 * 3600);
	long hour = length / 3600;

	length %= 3600;
	long minutes = length / 60;

	length %= 60;
	long seconds = length;

    QString fl;

    if (day > 0) { fl += QString("%1d:").arg(day); }
    if (hour > 0) { fl += QString("%1h:").arg(hour); }
    if (minutes > 0 && seconds < 1) { fl += QString("%1m").arg(minutes); } else { fl += QString("%1m:").arg(minutes); }
    if (seconds > 0) { fl += QString("%1s").arg(seconds); }

	return  fl;
}
