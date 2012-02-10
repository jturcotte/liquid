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

#ifndef TABMANAGER_H
#define TABMANAGER_H

#include "qobjectlistmodel.h"
#include <QDeclarativeImageProvider>
#include <QLinkedList>
#include <QUrl>

class Backend;
class Tab;
class TabManager;
class QDeclarativeEngine;

class TabsImageProvider : public QDeclarativeImageProvider
{
public:
    TabsImageProvider(TabManager* tabManager);
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);

private:
    TabManager* m_tabManager;
};


class TabStats : public QObject {
    Q_OBJECT
public:
    double valueForTab(Tab* tab);
    void onTabNavigated(Tab* tab);

signals:
    void valuesChanged();

private:
    QList<Tab*> m_navigations;
};


class TabManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObjectListModel* tabs READ tabs CONSTANT)
    Q_PROPERTY(Tab* currentTab READ currentTab WRITE setCurrentTab NOTIFY currentTabChanged)

public:
    explicit TabManager(Backend* backend);
    QObjectListModel* tabs() { return &m_tabs; }

    Backend* backend() const { return m_backend; }
    Tab* currentTab() const { return m_lastCurrentTabs.isEmpty() ? 0 : m_lastCurrentTabs.last(); }
    TabStats* tabStats() { return &m_tabStats; }
    QDeclarativeEngine* engine() const { return m_engine; }
    void setCurrentTab(Tab* tab);
    Q_INVOKABLE Tab* addNewTab(Tab* parentTab = 0, QUrl url = QUrl());
    Q_INVOKABLE void showNextTab();
    Q_INVOKABLE void showPreviousTab();
    void onTabClosed(Tab* tab);
    void initializeEngine(QDeclarativeEngine *engine);

#ifdef WK2_BUILD
    QWKContext* webContext() const { return m_webContext; }
#endif

signals:
    void currentTabChanged();

private:
#ifdef WK2_BUILD
    QWKContext* m_webContext;
#endif
    Backend* m_backend;
    QObjectListModel m_tabs;
    QLinkedList<Tab*> m_lastCurrentTabs;
    TabsImageProvider* m_tabsImageProvider;
    QDeclarativeEngine* m_engine;
    TabStats m_tabStats;
};

#endif // TABMANAGER_H
