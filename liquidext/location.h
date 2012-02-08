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

#ifndef LOCATION_H
#define LOCATION_H

#include <QObject>
#include <QString>
#include <QUrl>

class Location : public QObject {
    Q_OBJECT

    Q_ENUMS(Type)
    Q_PROPERTY(QUrl destination READ destination WRITE setDestination NOTIFY destinationChanged)
    Q_PROPERTY(QString inputText READ inputText WRITE setInputText NOTIFY inputTextChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(Type type READ type WRITE setType NOTIFY typeChanged)
public:
    enum Type { WwwType, HistoryType, SearchType };

    Location()
    : m_type(WwwType)
    {
    }
    Location(const QUrl& destination, const QString& inputText, const QString& title, Type type)
        : m_destination(destination)
        , m_inputText(inputText)
        , m_title(title)
        , m_type(type)
    {
    }

    QUrl destination() const { return m_destination; }
    void setDestination(const QUrl& destination) { m_destination = destination; destinationChanged(); }
    QString inputText() const { return m_inputText; }
    void setInputText(const QString& inputText) { m_inputText = inputText; inputTextChanged(); }
    QString title() const { return m_title; }
    void setTitle(const QString& title) { m_title = title; titleChanged(); }
    Type type() const { return m_type; }
    void setType(Type type) { m_type = type; typeChanged(); }

signals:
    void destinationChanged();
    void inputTextChanged();
    void titleChanged();
    void typeChanged();

private:
    QUrl m_destination;
    QString m_inputText;
    QString m_title;
    Type m_type;
};

#endif // LOCATION_H
