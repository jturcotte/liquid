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

#include "searchlocator.h"

#include "location.h"
#include <QNetworkReply>
#include <QXmlSimpleReader>

bool SuggestionXmlHandler::startElement(const QString&, const QString& localName, const QString&, const QXmlAttributes& atts)
{
    if (localName == QLatin1String("suggestion"))
        emit suggestionFound(atts.value(QLatin1String("data")));
    return true;
}

SearchLocator::SearchLocator()
{
     connect(&m_qnam, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
     connect(&m_xmlHandler, SIGNAL(suggestionFound(const QString&)), this, SLOT(appendSuggestion(const QString&)));
}

void SearchLocator::setQueryText(const QString& text)
{
    m_queryText = text;
    QUrl url(QLatin1String("http://google.com/complete/search?output=toolbar&q=") + text);
    m_qnam.get(QNetworkRequest(url));
}

QUrl SearchLocator::searchUrlFromUserInput(const QString& input)
{
    QString inputCopy(input);
    return QUrl("http://www.google.com/search?q=" + inputCopy.replace(' ', '+'));
}

void SearchLocator::replyFinished(QNetworkReply* reply)
{
    m_results.clear();
    QXmlSimpleReader xmlReader;
    QXmlInputSource *source = new QXmlInputSource(reply);
    xmlReader.setContentHandler(&m_xmlHandler);
    bool ok = xmlReader.parse(source);
    if (!ok)
        qWarning("Invalid suggestion XML received.");
    emit resultsChanged();
}

void SearchLocator::appendSuggestion(const QString& text)
{
    // FIXME: Make sure that those objects are deleted once we got a proper abstract model implementation.
    if (text != m_queryText && m_results.size() < 4)
        m_results << new Location(searchUrlFromUserInput(text), text, QLatin1String("Google Search"), Location::SearchType);
}
