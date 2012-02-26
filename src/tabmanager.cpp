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

#include "tabmanager.h"

#include "backend.h"
#include "historyitem.h"
#include <cfloat>
#include <cmath>
#include <QDesktopServices>
#include <QtDeclarative/qdeclarativeengine.h>

static const int aliveTabsLimit = 30;
static const int closedTabsLimit = 5;

static QString tabsBackupPath()
{
    return QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/tabs.dat";
}


TabsImageProvider::TabsImageProvider()
    : QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap)
{
}

QPixmap TabsImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    QPixmap icon;
    if (id == "defaultIcon")
        icon = QWebSettings::webGraphic(QWebSettings::DefaultFrameIconGraphic);
    else {
        QUrl originalUrl = QUrl::fromEncoded(id.toLatin1());
        icon = QWebSettings::iconForUrl(originalUrl).pixmap(16);
    }
    *size = icon.size();
    return requestedSize.isValid() ? icon.scaled(requestedSize) : icon;
}

QUrl TabsImageProvider::iconSourceForUrl(const QUrl& originalUrl)
{
    QUrl url(originalUrl);
    bool hasIcon = !QWebSettings::iconForUrl(url).isNull();
    if (!hasIcon) {
        // The icon database isn't updated when navigating within the same document, try without the fragment in that case.
        url.setFragment(QString());
        hasIcon = !QWebSettings::iconForUrl(url).isNull();
    }
    return hasIcon ? QUrl("image://tabs/" + url.toEncoded()) : QUrl("image://tabs/defaultIcon");
}

double TabStats::valueForTab(Tab* tab)
{
    const double base = 1.1;
    int size = m_navigations.size();
    double total = (pow(base, size + 1) - base) / (base - 1);
    double value = 0.0;
    int currentIndex = -1;
    while ((currentIndex = m_navigations.indexOf(tab, currentIndex+1)) != -1)
        value += pow(base, currentIndex + 1);
    return total ? value / total : 0.0;
}

void TabStats::onTabNavigated(Tab* tab)
{
    m_navigations.append(tab);
    while (m_navigations.size() > 1000)
        m_navigations.removeFirst();
    emit valuesChanged();
}

TabManager::TabManager(Backend* backend)
    : QObject(backend)
    , m_backend(backend)
    , m_tabsImageProvider(new TabsImageProvider)
    , m_engine(0)
    , m_saveTabsPending(false)
{
}

void TabManager::setCurrentTab(Tab* tab)
{
    if (!m_lastCurrentTabs.isEmpty() && m_lastCurrentTabs.last() == tab)
        return;
    m_lastCurrentTabs.removeAll(tab);
    m_lastCurrentTabs.append(tab);
    emit currentTabChanged();
    saveTabs();
}

void TabManager::showNextTab()
{
    int currentTabIndex = m_tabs.indexOf(currentTab());
    int tabIndex = currentTabIndex;
    do
        tabIndex = tabIndex == m_tabs.size() - 1 ? 0 : tabIndex + 1;
    while (static_cast<Tab*>(m_tabs.at(tabIndex))->closed() && tabIndex != currentTabIndex);
    if (tabIndex != currentTabIndex)
        setCurrentTab(static_cast<Tab*>(m_tabs.at(tabIndex)));
}

void TabManager::showPreviousTab()
{
    int currentTabIndex = m_tabs.indexOf(currentTab());
    int tabIndex = currentTabIndex;
    do
        tabIndex = tabIndex == 0 ? m_tabs.size() - 1 : tabIndex - 1;
    while (static_cast<Tab*>(m_tabs.at(tabIndex))->closed() && tabIndex != currentTabIndex);
    if (tabIndex != currentTabIndex)
        setCurrentTab(static_cast<Tab*>(m_tabs.at(tabIndex)));
}

void TabManager::onTabClosed(Tab* tab)
{
    bool wasCurrentTab = currentTab() == tab;
    m_lastCurrentTabs.removeAll(tab);
    if (wasCurrentTab)
        emit currentTabChanged();

    // Check if we passed the limit of closed tabs.
    double cheapestWeight = DBL_MAX;
    int cheapestClosedTabIndex = -1;
    int numClosedTabs = 0;
    for (int i = 0; i < m_tabs.size(); ++i) {
        Tab* tab = static_cast<Tab*>(m_tabs.at(i));
        if (tab->closed()) {
            ++numClosedTabs;
            if (tab->baseWeight() < cheapestWeight) {
                cheapestWeight = tab->baseWeight();
                cheapestClosedTabIndex = i;
            }
        }
    }
    if (numClosedTabs > closedTabsLimit && cheapestClosedTabIndex != -1) {
        Q_ASSERT(numClosedTabs == closedTabsLimit + 1);
        m_tabs.removeAt(cheapestClosedTabIndex);
    }
    saveTabs();
}

void TabManager::saveTabs()
{
    if (m_saveTabsDeferTimer.isActive()) {
        m_saveTabsPending = true;
        return;
    }

    QFile file(tabsBackupPath());
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);

    out << m_tabs.size();
    out << m_tabs.indexOf(currentTab());

    // FIXME: Save the navigation history and the tab stats as well.
    for (int i = 0; i < m_tabs.size(); ++i) {
        Tab* tab = static_cast<Tab*>(m_tabs.at(i));
        out << tab->url().toEncoded();
        out << tab->closed();
    }

    m_saveTabsDeferTimer.start(10000, this);
    m_saveTabsPending = false;
}

bool TabManager::restoreTabs()
{
    QFile file(tabsBackupPath());
    if (!file.open(QIODevice::ReadOnly))
        return false;
    QDataStream in(&file);

    int numTabs, currentTabIndex;
    in >> numTabs;
    in >> currentTabIndex;

    QVector<std::pair<QUrl, bool> > unserialized(numTabs);
    for (int i = 0; i < unserialized.size(); ++i) {
        in >> unserialized[i].first;
        in >> unserialized[i].second;
    }
    if (in.status() != QDataStream::Ok) {
        qWarning("WARNING: Corrupted persisted tabs file, ignoring.");
        return false;
    }

    // Do the restore, start the save deferrer to prevent saving an incomplete state.
    m_saveTabsDeferTimer.start(10000, this);
    for (int i = 0; i < unserialized.size(); ++i) {
        Tab* tab = new Tab(unserialized[i].first, i, 0, this, unserialized[i].second);
        m_tabs.append(tab);
        // Artificially fill the stack of current tabs.
        if (!tab->closed())
            m_lastCurrentTabs.append(tab);
    }

    // Set the real current tab.
    if (currentTabIndex >= 0 && currentTabIndex < m_tabs.size())
        m_lastCurrentTabs.append(static_cast<Tab*>(m_tabs.at(currentTabIndex)));
    emit currentTabChanged();
    return true;
}

void TabManager::initializeEngine(QDeclarativeEngine *engine)
{
    engine->addImageProvider(QLatin1String("tabs"), m_tabsImageProvider);
    m_engine = engine;
}

Tab* TabManager::addNewTab(Tab* parentTab, QUrl url)
{
    int newTabPos = m_tabs.size();
    double newTabIndex = 0;

    if (parentTab) {
        int parentTabPos = m_tabs.indexOf(parentTab);
        newTabPos = parentTabPos + 1;
        if (parentTabPos + 1 != m_tabs.size())
            newTabIndex = (parentTab->index() + static_cast<Tab*>(m_tabs.at(parentTabPos+1))->index()) / 2;
        else
            newTabIndex = parentTab->index() + 1;
    } else if (!m_tabs.isEmpty())
        newTabIndex = static_cast<Tab*>(m_tabs.last())->index() + 1;

    Tab* newTab = new Tab(url, newTabIndex, parentTab, this);
    m_tabs.insert(newTabPos, newTab);

    // Check if we passed the limit of tabs.
    double cheapestWeight;
    Tab* cheapestTab;
    int numAliveTabs;
    while (true) {
        cheapestWeight = DBL_MAX;
        cheapestTab = 0;
        numAliveTabs = 0;
        for (int i = 0; i < m_tabs.size(); ++i) {
            Tab* tab = static_cast<Tab*>(m_tabs.at(i));
            if (!tab->closed()) {
                ++numAliveTabs;
                if (tab->baseWeight() < cheapestWeight) {
                    cheapestWeight = tab->baseWeight();
                    cheapestTab = tab;
                }
            }
        }
        if (numAliveTabs <= aliveTabsLimit)
            break;
        if (cheapestTab)
            cheapestTab->close();
    }
    saveTabs();
    return newTab;
}

void TabManager::timerEvent(QTimerEvent* event)
{
    if (event->timerId() != m_saveTabsDeferTimer.timerId()) {
        QObject::timerEvent(event);
        return;
    }
    m_saveTabsDeferTimer.stop();
    if (m_saveTabsPending)
        saveTabs();
}
