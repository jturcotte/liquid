/*
    Copyright (C) 2011 Jocelyn Turcotte <turcotte.j@gmail.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this program; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef MOUSEWHEELAREA_H
#define MOUSEWHEELAREA_H

#include <QtDeclarative/QDeclarativeItem>
#include <QGraphicsSceneWheelEvent>
#include <QDebug>

class MouseWheelArea : public QDeclarativeItem
{
    Q_OBJECT
    Q_DISABLE_COPY(MouseWheelArea)

public:
    MouseWheelArea(QDeclarativeItem *parent = 0);
    ~MouseWheelArea();
    void wheelEvent(QGraphicsSceneWheelEvent* event)
    {
        emit wheel(event->delta(), event->pos().x(), event->pos().y());
    }

signals:
    void wheel(int delta, qreal mouseX, qreal mouseY);
};

QML_DECLARE_TYPE(MouseWheelArea)


#endif // MOUSEWHEELAREA_H
