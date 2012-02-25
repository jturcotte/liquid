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

#include "tab.h"

#include "backend.h"
#include "historydatabase.h"
#include "historyitem.h"
#include "tabmanager.h"
#include <QtDeclarative/qdeclarativeengine.h>
#include <QWebFrame>

WebPage::WebPage(Tab* tab)
: QDeclarativeWebPage(tab)
, m_tab(tab)
{
}

QWebPage* WebPage::createWindow(WebWindowType)
{
    Tab* newTab = m_tab->manager()->addNewTab(m_tab);
    m_tab->manager()->setCurrentTab(newTab);
    return newTab->webView()->page();
}

QWebPage* WebPage::createBackgroundWindow(WebWindowType)
{
    Tab* newTab = m_tab->manager()->addNewTab(m_tab);
    return newTab->webView()->page();
}

bool WebPage::supportsExtension(QWebPage::Extension extension) const
{
    return extension == QWebPage::ErrorPageExtension;
}

bool WebPage::extension(Extension, const ExtensionOption* option, ExtensionReturn* output)
{
    static_cast<ErrorPageExtensionReturn*>(output)->content = QString("<h3>%1</h3><p>Code: %2</p>")
            .arg(static_cast<const ErrorPageExtensionOption*>(option)->errorString)
            .arg(static_cast<const ErrorPageExtensionOption*>(option)->error).toUtf8();
    return true;
}


Tab::Tab(const QUrl& url, double index, TabManager* manager, bool closed, QObject* parent)
    : QObject(parent)
    , m_pendingUrl(url)
    , m_index(index)
    , m_manager(manager)
    , m_webPage(0)
    , m_webView(0)
    , m_currentHistoryItem(0)
    , m_storedVisit(false)
{
    connect(m_manager->tabStats(), SIGNAL(valuesChanged()), SIGNAL(baseWeightChanged()));
    if (!closed)
        webView();
}

Tab::~Tab()
{
    delete m_webView;
    delete m_webPage;
}

WebView* Tab::webView()
{
    Q_ASSERT_X(m_manager->engine(), "Tab::webView()", "This shouldn't be called before the declarative engine was initialized.");
    // We manage the page's lifetime,
    if (!m_webPage) {
        m_webPage = new WebPage(this);
        m_webPage->setNetworkAccessManager(m_manager->engine()->networkAccessManager());
    }
    if (!m_webView) {
        m_webView = new WebView;
        m_webView->setPage(m_webPage);
        connect(m_webView, SIGNAL(titleChanged(const QString&)), SIGNAL(titleChanged()));
        connect(m_webView, SIGNAL(titleChanged(const QString&)), SLOT(updateTitle()));
        connect(m_webView, SIGNAL(iconChanged()), SLOT(onIconChanged()));
        connect(m_webView, SIGNAL(iconChanged()), SIGNAL(iconSourceChanged()));
        connect(m_webView, SIGNAL(loadStarted()), SLOT(onLoadStarted()));
        connect(m_webView, SIGNAL(loadFinished()), SLOT(onLoadFinished()));
        connect(m_webView, SIGNAL(loadFinished()), SLOT(updateHistory()));
        connect(m_webView, SIGNAL(urlChanged()), SLOT(updateHistory()));
        // The web view might get deleted when reloading in QmlViewer.
        connect(m_webView, SIGNAL(destroyed()), SLOT(webViewDestroyed()));
        connect(m_webPage->mainFrame(), SIGNAL(urlChanged(const QUrl&)), SLOT(onNavigation()));
        emit closedChanged();
    }
    if (m_pendingUrl.isValid()) {
        QUrl pendingUrl = m_pendingUrl;
        m_pendingUrl = QUrl();
        m_webView->setUrl(pendingUrl);
    }
    return m_webView;
}

QUrl Tab::iconSource() const
{
    QUrl url = m_webView ? m_webView->url() : m_pendingUrl;
    return (!QWebSettings::iconForUrl(url).isNull()) ? QUrl("image://tabs/" + url.toEncoded()) : QUrl("image://tabs/defaultIcon");
}

double Tab::baseWeight()
{
    return m_manager->tabStats()->valueForTab(this);
}

void Tab::loadUrl(const QUrl& url, const QString& typedText)
{
    m_locationSubstringTyped = typedText;
    m_webView->setUrl(url);
}

void Tab::close()
{
    if (m_webView) {
        // TODO: Keep the back/forward history.
        m_pendingUrl = m_webView->url();
        delete m_webView;
        delete m_webPage;
        m_webView = 0;
        m_webPage = 0;
        emit closedChanged();
    }
    m_manager->onTabClosed(this);
}

void Tab::updateHistory()
{
    // FIXME: This whole thing will break once we hit the back/forward history capacity limit (Default to 100)
    bool shouldEmit = false;
    if (m_history.count() != m_webView->history()->count())
        shouldEmit = true;
    for (int i = m_history.count(); i > m_webView->history()->count(); --i)
        m_history.removeLast();
    for (int i = m_history.count(); i < m_webView->history()->count(); ++i)
        m_history.append(new HistoryItem(i, this, this));
    for (int i = 0; i < m_webView->history()->count(); ++i)
        static_cast<HistoryItem*>(m_history.at(i))->checkRelativeIndex();
    if (shouldEmit)
        emit historyChanged();
    if (m_webView->history()->currentItemIndex() < m_history.size())
        m_currentHistoryItem = static_cast<HistoryItem*>(m_history.at(m_webView->history()->currentItemIndex()));
}

void Tab::updateTitle()
{
    if (!m_webView->title().isEmpty() && m_storedVisit)
        m_manager->backend()->historyDatabase()->updateTitleForUrl(m_webView->title(), m_webView->url());
    m_lastTitle = m_webView->title();
    if (m_currentHistoryItem)
        emit m_currentHistoryItem->titleChanged();
}

void Tab::webViewDestroyed()
{
    m_webView = 0;
}

void Tab::onNavigation()
{
    m_manager->tabStats()->onTabNavigated(this);
    m_manager->saveTabs();
}

void Tab::onLoadStarted()
{
    m_storedVisit = false;
}

static bool storeVisit(const QUrl& url)
{
    // Seems to cover most cases.
    return !url.path().startsWith("/search")
        && url.scheme() != "about";
}

void Tab::onLoadFinished()
{
    if (!m_storedVisit) {
        if (storeVisit(m_webView->url())) {
            m_manager->backend()->historyDatabase()->addVisitedUrl(m_webView->url(), m_webView->page()->mainFrame()->requestedUrl(), m_locationSubstringTyped);
            if (!m_webView->title().isEmpty())
                m_manager->backend()->historyDatabase()->updateTitleForUrl(m_webView->title(), m_webView->url());
            m_storedVisit = true;
        }
    }
}

void Tab::onIconChanged()
{
    // Not efficient, but should do.
    for (int i = 0; i < m_history.size(); ++i)
        static_cast<HistoryItem*>(m_history[i])->checkIcon();
}
