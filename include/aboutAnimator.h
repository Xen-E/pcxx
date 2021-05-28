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


#ifndef ABOUTANIMATOR_H
#define ABOUTANIMATOR_H

#include <types.h>
#include <QThread>

class Animator : public QObject
{
    Q_OBJECT

signals:
    void UpdateAboutDescription(const QChar &letter);

public slots:
    void Animate()
    {
        foreach (const QChar &letter, description) {
            QThread::msleep(speed);
            UpdateAboutDescription(letter);
        }
    }

public:
    QString description;
    int speed;
    virtual ~Animator() { delete this; }
};

#endif // ABOUTANIMATOR_H
