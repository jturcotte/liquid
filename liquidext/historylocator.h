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

#ifndef HISTORYLOCATOR_H
#define HISTORYLOCATOR_H

#include "location.h"
#include <QObject>
#include <QScopedPointer>

class HistoryDatabase;
class Location;

class HistoryLocator : public QObject {
    Q_OBJECT
    Q_PROPERTY(QList<QObject*> results READ results NOTIFY resultsChanged)
    Q_PROPERTY(Location* hostCompletionLocation READ hostCompletionLocation NOTIFY hostCompletionLocationChanged)
public:
    HistoryLocator(HistoryDatabase* database);
    Q_INVOKABLE void setQueryText(const QString& text);
    QList<QObject*> results() { return m_results; }
    Location* hostCompletionLocation() { return m_hostCompletionLocation.data(); }

signals:
    void resultsChanged();
    void hostCompletionLocationChanged();

private:
    HistoryDatabase* m_database;
    QList<QObject*> m_results;
    QScopedPointer<Location> m_hostCompletionLocation;
};

#endif // HISTORYLOCATOR_H
