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
#include <cmath>
#include <QtDeclarative/qdeclarativeengine.h>

TabsImageProvider::TabsImageProvider(TabManager* tabManager)
    : QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap)
    , m_tabManager(tabManager)
{
}

QPixmap TabsImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    QPixmap icon;
    if (id == "defaultIcon")
        icon = QWebSettings::webGraphic(QWebSettings::DefaultFrameIconGraphic);
    else {
        QUrl originalUrl = QUrl(id);
        icon = QWebSettings::iconForUrl(originalUrl).pixmap(16);
    }
    *size = icon.size();
    return requestedSize.isValid() ? icon.scaled(requestedSize) : icon;
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
#ifdef WK2_BUILD
    , m_webContext(new QWKContext)
#endif
    // , m_tabs(new QList<QObject*>)
    , m_tabsImageProvider(new TabsImageProvider(this))
    , m_engine(0)
{
}

void TabManager::setCurrentTab(Tab* tab)
{
    if (!m_lastCurrentTabs.isEmpty() && m_lastCurrentTabs.last() == tab)
        return;
    m_lastCurrentTabs.removeAll(tab);
    m_lastCurrentTabs.append(tab);
    emit currentTabChanged();
}

void TabManager::showNextTab()
{
    if (m_tabs.size() <= 1)
        return;

    int currentTabIndex = m_tabs.indexOf(currentTab());
    int tabIndex = currentTabIndex == m_tabs.size() - 1 ? 0 : currentTabIndex + 1;
    setCurrentTab(static_cast<Tab*>(m_tabs.at(tabIndex)));
}

void TabManager::showPreviousTab()
{
    if (m_tabs.size() <= 1)
        return;

    int currentTabIndex = m_tabs.indexOf(currentTab());
    int tabIndex = currentTabIndex == 0 ? m_tabs.size() - 1 : currentTabIndex - 1;
    setCurrentTab(static_cast<Tab*>(m_tabs.at(tabIndex)));
}

void TabManager::onTabClosed(Tab* tab)
{
    bool wasCurrentTab = currentTab() == tab;
    m_lastCurrentTabs.removeAll(tab);
    if (wasCurrentTab)
        emit currentTabChanged();
}

void TabManager::initializeEngine(QDeclarativeEngine *engine)
{
    engine->addImageProvider(QLatin1String("tabs"), m_tabsImageProvider);
    m_engine = engine;
}

Tab* TabManager::addNewTab(QUrl url)
{
    Tab* newTab = new Tab(url, m_tabs.size(), this);
    m_tabs.append(newTab);
    return newTab;
}
