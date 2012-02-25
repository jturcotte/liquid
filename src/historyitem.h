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

#ifndef HISTORYITEM_H
#define HISTORYITEM_H

#include <QObject>
#include "tab.h"

class HistoryItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl iconSource READ iconSource NOTIFY iconSourceChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(bool current READ current NOTIFY currentChanged)
    Q_PROPERTY(bool linking READ linking CONSTANT)

public:
    HistoryItem(int qWebHistoryIndex, Tab* tab, QObject* parent = 0);

    Q_INVOKABLE void goTo();
    QUrl iconSource();
    QString title();
    bool current() const;
    bool linking() const { return false; }

    void checkIcon();
    void checkRelativeIndex();

signals:
    void iconSourceChanged();
    void titleChanged();
    void currentChanged();

private:
    int m_qWebHistoryIndex;
    Tab* m_tab;
    friend class Tab;
};

class ParentTabHistoryItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl iconSource READ iconSource NOTIFY iconSourceChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(bool current READ current CONSTANT)
    Q_PROPERTY(bool linking READ linking CONSTANT)

public:
    ParentTabHistoryItem(Tab* tab, Tab* parentTab, QObject* parent = 0);

    Q_INVOKABLE void goTo();
    QUrl iconSource();
    QString title();
    bool current() const { return false; }
    bool linking() const { return true; }

    void checkIcon();
    void checkRelativeIndex();

signals:
    void iconSourceChanged();
    void titleChanged();

private:
    Tab* m_tab;
    Tab* m_parentTab;
};

#endif // HISTORYITEM_H
