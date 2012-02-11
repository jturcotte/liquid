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

#include "backend.h"

#include <QAuthenticator>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QNetworkProxyFactory>
#include <QNetworkReply>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include "cookiejar.h"
#include "location.h"
#include "tabmanager.h"

Backend::Backend(QObject *parent)
    : QObject(parent)
    , m_tabManager(new TabManager(this))
    , m_historyDatabase(new HistoryDatabase)
    , m_historyLocator(m_historyDatabase)
{
}

Backend::~Backend()
{
    delete m_historyDatabase;
    delete m_tabManager;
}

Location* Backend::locationFromUserInput(const QString& input)
{
    // The returned object is leaked and should automatically have a JavaScript ownership when called from QML.
    QUrl straightUrl = QUrl(input);
    QUrl url = QUrl::fromUserInput(input);
    bool isSearchInput = straightUrl.scheme() != "http" && straightUrl.scheme() != "https" && url.scheme() != "file" && !url.host().contains('.');
    if (isSearchInput)
        return new Location(SearchLocator::searchUrlFromUserInput(input), input, QLatin1String("Google Search"), Location::SearchType);
    else {
        QUrl destination = QUrl::fromUserInput(input);
        return new Location(destination, input, destination.toString(), Location::WwwType);
    }
}

QString Backend::machineUsername()
{
    return getenv("USERNAME");
}

void Backend::initializeEngine(QDeclarativeEngine *engine)
{
    QCoreApplication::setOrganizationName(QLatin1String("liquid"));
    QCoreApplication::setApplicationName(QLatin1String("liquid"));

    QWebSettings* settings = QWebSettings::globalSettings();
    settings->setAttribute(QWebSettings::DnsPrefetchEnabled, true);
    settings->setAttribute(QWebSettings::PluginsEnabled, false);
    settings->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    settings->setAttribute(QWebSettings::JavascriptCanAccessClipboard, true);
    settings->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    settings->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, true);
    settings->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled, true);
    settings->setAttribute(QWebSettings::LocalStorageEnabled, true);
    settings->setAttribute(QWebSettings::AcceleratedCompositingEnabled, true);
    QWebSettings::enablePersistentStorage();

    QString cookieJarPath = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/cookiejar.dat";
    engine->networkAccessManager()->setCookieJar(new CookieJar(cookieJarPath, engine->networkAccessManager()));
    connect(engine->networkAccessManager(), SIGNAL(authenticationRequired(QNetworkReply*, QAuthenticator*)), SLOT(requestCredentials(QNetworkReply*, QAuthenticator*)));
    connect(engine->networkAccessManager(), SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError>&)), SLOT(ignoreSslErrors(QNetworkReply*)));
    QNetworkProxyFactory::setUseSystemConfiguration(true);

    engine->rootContext()->setContextProperty(QLatin1String("backend"), this);
    m_tabManager->initializeEngine(engine);
}

void Backend::requestCredentials(QNetworkReply* reply, QAuthenticator* authenticator)
{
    emit credentialsNeeded();
    int cancelled = m_credentialsEventLoop.exec(QEventLoop::ExcludeSocketNotifiers);
    if (cancelled) {
        reply->abort();
    } else {
        authenticator->setUser(m_enteredUsername);
        authenticator->setPassword(m_enteredPassword);
        m_enteredUsername = QString();
        m_enteredPassword = QString();
    }
}

void Backend::enterCredentials(const QString& username, const QString& password)
{
    m_enteredUsername = username;
    m_enteredPassword = password;
    m_credentialsEventLoop.exit(0);
}

void Backend::cancelAuth()
{
    m_credentialsEventLoop.exit(1);
}

void Backend::ignoreSslErrors(QNetworkReply* reply)
{
    // FIXME!!!: Give the user proper feedback and information about this trust issue.
    reply->ignoreSslErrors();
}
