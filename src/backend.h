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

#ifndef BACKEND_H
#define BACKEND_H

#include "historydatabase.h"
#include "historylocator.h"
#include "searchlocator.h"
#include "tab.h"
#include <QEventLoop>
#include <QObject>

class Location;
class QAuthenticator;

class Backend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(TabManager* tabManager READ tabManager CONSTANT)
    Q_PROPERTY(HistoryLocator* historyLocator READ historyLocator CONSTANT)
    Q_PROPERTY(SearchLocator* searchLocator READ searchLocator CONSTANT)
    Q_PROPERTY(bool isMac READ isMac CONSTANT)
public:
    explicit Backend(QObject *parent = 0);
    ~Backend();

    TabManager* tabManager() { return m_tabManager; }
    HistoryDatabase* historyDatabase() { return m_historyDatabase; }
    HistoryLocator* historyLocator() { return &m_historyLocator; }
    SearchLocator* searchLocator() { return &m_searchLocator; }
    bool isMac() const;
    Q_INVOKABLE Location* locationFromUserInput(const QString& input);
    Q_INVOKABLE QString machineUsername();
    void initializeEngine(QDeclarativeEngine *engine);

signals:
    void credentialsNeeded();

public slots:
    void requestCredentials(QNetworkReply* reply, QAuthenticator* authenticator);
    void enterCredentials(const QString& username, const QString& password);
    void cancelAuth();

private slots:
    void ignoreSslErrors(QNetworkReply* reply);

private:
    TabManager* m_tabManager;
    HistoryDatabase* m_historyDatabase;
    HistoryLocator m_historyLocator;
    SearchLocator m_searchLocator;
    QEventLoop m_credentialsEventLoop;
    QString m_enteredUsername;
    QString m_enteredPassword;
};

QML_DECLARE_TYPE(Backend)

#endif // BACKEND_H
