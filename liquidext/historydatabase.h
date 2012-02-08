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

#ifndef HISTORYDATABASE_H
#define HISTORYDATABASE_H

#include <QObject>
#include <QSqlDatabase>

class HistoryLocator;
class Location;
class QUrl;
class QString;

class HistoryDatabase : public QObject {
    Q_OBJECT
public:
    HistoryDatabase();
    void addVisitedUrl(const QUrl& url, const QUrl& requestedUrl, const QString& typedText);
    void updateTitleForUrl(const QString& title, const QUrl& url);
    QList<QObject*> suggestionsForText(const QString& text);
    Location* hostCompletionForText(const QString& text);

private:
    QSqlDatabase m_db;
    friend class HistoryLocator;
};

#endif // HISTORYDATABASE_H
