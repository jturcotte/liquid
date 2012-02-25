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

#include "historydatabase.h"

#include "location.h"
#include <QDesktopServices>
#include <QSqlError>
#include <QSqlQuery>
#include <QUrl>
#include <QVariant>

HistoryDatabase::HistoryDatabase()
    : m_db(QSqlDatabase::addDatabase("QSQLITE"))
{
    QString databasePath = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QLatin1String("/history.db");
    m_db.setDatabaseName(databasePath);
    if (!m_db.open())
        qFatal("Can't open database.");
    m_db.exec("CREATE TABLE IF NOT EXISTS visited_pages ("
              "page_id INTEGER PRIMARY KEY,"
              "url TEXT UNIQUE NOT NULL,"
              "title TEXT NOT NULL DEFAULT '',"
              "is_host_only BOOLEAN NOT NULL,"
              "num_visits INTEGER)");

    m_db.exec("CREATE TABLE IF NOT EXISTS typed_requests ("
              "typed_text TEXT NOT NULL,"
              "visited_page_id INTEGER NOT NULL,"
              "num_visits INTEGER NOT NULL,"
              "PRIMARY KEY (typed_text, visited_page_id),"
              "FOREIGN KEY (visited_page_id) REFERENCES visited_pages (page_id))");
}

static void displayFailure(const QSqlQuery& query)
{
    qFatal("Error: [%s] executing SQL query: [%s]", qPrintable(query.lastError().text()), qPrintable(query.executedQuery()));
}

void HistoryDatabase::addVisitedUrl(const QUrl& url, const QUrl& requestedUrl, const QString& typedText)
{
    bool isHostOnly = requestedUrl.path().length() <= 1;

    m_db.transaction();
    QSqlQuery query(m_db);
    query.prepare("INSERT OR IGNORE INTO visited_pages (url, is_host_only, num_visits) VALUES (?, ?, 0)");
    // When a requested url is a host only, store the entered "alias" instead of the redirected URL.
    query.addBindValue(isHostOnly ? requestedUrl.toEncoded() : url.toEncoded());
    query.addBindValue(QVariant(isHostOnly).toInt());
    if (!query.exec())
        displayFailure(query);

    query.prepare("INSERT OR IGNORE INTO typed_requests "
                  "(typed_text, visited_page_id, num_visits) "
                  "SELECT ?, page_id, 0 FROM visited_pages WHERE url = ?");
    query.addBindValue(typedText);
    query.addBindValue(url.toEncoded());
    if (!query.exec())
        displayFailure(query);

    query.prepare("UPDATE visited_pages SET num_visits = num_visits + 1 WHERE url = ?");
    query.addBindValue(url.toEncoded());
    if (!query.exec())
        displayFailure(query);

    query.prepare("UPDATE typed_requests SET num_visits = num_visits + 1 "
                  "WHERE typed_text = ? "
                  "AND visited_page_id = (SELECT page_id FROM visited_pages WHERE url = ?)");
    query.addBindValue(typedText);
    query.addBindValue(url.toEncoded());
    if (!query.exec())
        displayFailure(query);
    m_db.commit();
}

void HistoryDatabase::updateTitleForUrl(const QString& title, const QUrl& url)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE visited_pages SET title = ? WHERE url = ?");
    query.addBindValue(title);
    query.addBindValue(url.toEncoded());
    if (!query.exec())
        displayFailure(query);
}

QList<QObject*> HistoryDatabase::suggestionsForText(const QString& text)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT url, title "
                  "FROM visited_pages p LEFT JOIN typed_requests r "
                  "ON r.visited_page_id = p.page_id AND r.typed_text LIKE ? "
                  "WHERE title LIKE ? OR url LIKE ? "
                  "GROUP BY url "
                  // Score is given by adding the number of visits + a bonus for the similarity
                  // of the currently entered text vs. what was used in the past.
                  "ORDER BY p.num_visits + total(? / length(typed_text) * r.num_visits) DESC "
                  "LIMIT 4");
    query.addBindValue(QString("%1%%").arg(text));
    query.addBindValue(QString("%%%1%%").arg(text));
    query.addBindValue(QString("%%%1%%").arg(text));
    query.addBindValue(QVariant(text.size()).toDouble());
    if (!query.exec())
        displayFailure(query);
    QList<QObject*> results;
    while (query.next())
        // Leaks the objects to the caller.
        results << new Location(query.value(0).toString(), query.value(0).toString(), query.value(1).toString(), Location::HistoryType);
    return results;
}

Location* HistoryDatabase::hostCompletionForText(const QString& text)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT url, title "
                  "FROM visited_pages p LEFT JOIN typed_requests r "
                  "ON r.visited_page_id = p.page_id AND r.typed_text LIKE ? "
                  "WHERE is_host_only AND (url LIKE ? OR url LIKE ?) "
                  "GROUP BY url "
                  "ORDER BY p.num_visits + total(? / length(typed_text) * r.num_visits) DESC "
                  "LIMIT 1");
    query.addBindValue(QString("%1%%").arg(text));
    query.addBindValue(QString("http://%1%%").arg(text));
    query.addBindValue(QString("https://%1%%").arg(text));
    query.addBindValue(QVariant(text.size()).toDouble());
    if (!query.exec())
        displayFailure(query);
    if (query.next())
        // Leaks the object to the caller.
        return new Location(query.value(0).toString(), query.value(0).toString(), query.value(1).toString(), Location::WwwType);
    return 0;
}
