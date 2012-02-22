/*
    Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
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

#ifndef COOKIEJAR_H
#define COOKIEJAR_H

#include <QBasicTimer>
#include <QNetworkCookieJar>

class CookieJar : public QNetworkCookieJar
{
    Q_OBJECT
public:
    CookieJar(const QString& cookieFileName, QObject* parent = 0);
    virtual ~CookieJar();

    void save();
    void load();

    virtual bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url);

public Q_SLOTS:
    void clearCookieJar();

protected:
    virtual void timerEvent(QTimerEvent* ev);

private:
    void expireCookies();
    QBasicTimer m_cookieSavingTimer;
    QString m_cookieFileName;
};

#endif // COOKIEJAR_H
