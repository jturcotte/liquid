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

#ifndef TAB_H
#define TAB_H

#include "qdeclarativewebview_p.h"
#include "qobjectlistmodel.h"
#include <QWebHistory>

class TabManager;
class HistoryItem;
class Tab;

typedef QDeclarativeWebView WebView;


class WebPage : public QDeclarativeWebPage {
public:
    WebPage(Tab* tab);
    virtual QWebPage *createWindow(WebWindowType);
    virtual QWebPage *createBackgroundWindow(WebWindowType);
    bool supportsExtension(QWebPage::Extension extension) const;
    bool extension(Extension extension, const ExtensionOption* option = 0, ExtensionReturn* output = 0);
private:
    Tab* m_tab;
};


class Tab : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeWebView* webView READ webView CONSTANT)
    Q_PROPERTY(QObjectListModel* history READ history NOTIFY historyChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QUrl iconSource READ iconSource NOTIFY iconSourceChanged)
    Q_PROPERTY(double baseWeight READ baseWeight NOTIFY baseWeightChanged)
    Q_PROPERTY(bool closed READ closed NOTIFY closedChanged)
    Q_PROPERTY(double index READ index CONSTANT)

public:
    explicit Tab(const QUrl& url, double index, TabManager* manager, bool closed = false, QObject* parent = 0);
    ~Tab();

    double index() const { return m_index; }
    TabManager* manager() const { return m_manager; }
    WebView* webView();
    QString title() const { return m_webView ? m_webView->title() : m_lastTitle; }
    QUrl url() const { return m_webView ? m_webView->url() : m_pendingUrl; }
    bool closed() const { return !m_webView; }
    QUrl iconSource() const;
    double baseWeight();
    QObjectListModel* history() { return &m_history; }
    Q_INVOKABLE void loadUrl(const QUrl& url, const QString& typedText);
    Q_INVOKABLE void close();

signals:
    void historyChanged();
    void titleChanged();
    void closedChanged();
    void iconSourceChanged();
    void baseWeightChanged();
    void navigated();

private slots:
    void updateHistory();
    void updateTitle();
    void webViewDestroyed();
    void onNavigation();
    void onLoadStarted();
    void onLoadFinished();
    void onIconChanged();

private:
    QUrl m_pendingUrl;
    QString m_locationSubstringTyped;
    QString m_lastTitle;
    double m_index;
    TabManager* m_manager;
    WebPage* m_webPage;
    WebView* m_webView;
    QObjectListModel m_history;
    HistoryItem* m_currentHistoryItem;
    bool m_storedVisit;
};

#endif // TAB_H
