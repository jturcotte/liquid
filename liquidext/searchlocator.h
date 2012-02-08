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

#ifndef SEARCHLOCATOR_H
#define SEARCHLOCATOR_H

#include <QList>
#include <QNetworkAccessManager>
#include <QXmlDefaultHandler>

class QNetworkReply;

class SuggestionXmlHandler : public QObject, public QXmlDefaultHandler {
    Q_OBJECT
public:
    virtual bool startElement(const QString&, const QString& localName, const QString&, const QXmlAttributes& atts);

signals:
    void suggestionFound(const QString& text);
};

class SearchLocator : public QObject {
    Q_OBJECT
    Q_PROPERTY(QList<QObject*> results READ results NOTIFY resultsChanged)
public:
    SearchLocator();
    Q_INVOKABLE void setQueryText(const QString& text);
    QList<QObject*> results() { return m_results; }

    static QUrl searchUrlFromUserInput(const QString& input);

public slots:
    void replyFinished(QNetworkReply* reply);
    void appendSuggestion(const QString& text);

signals:
    void resultsChanged();

private:
    QString m_queryText;
    QNetworkAccessManager m_qnam;
    SuggestionXmlHandler m_xmlHandler;
    QList<QObject*> m_results;
};

#endif // SEARCHLOCATOR_H
