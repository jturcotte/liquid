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

#ifndef TABHISTORYLISTMODEL_H
#define TABHISTORYLISTMODEL_H

#include <QAbstractListModel>

class ParentTabHistoryItem;

class TabHistoryListModel : public QAbstractListModel, public QList<QObject*> {
    Q_OBJECT
public:
    TabHistoryListModel()
    : m_parentTabHistoryItem(0)
    {
        QHash<int, QByteArray> roles;
        roles[Qt::UserRole] = "modelObject";
        setRoleNames(roles);
    }

    // Subclass the whole model just to be able to prepend this custom history item.
    void setParentTabHistoryItem(ParentTabHistoryItem* item);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    int rowCount(const QModelIndex&) const
    {
        return modelOffset() + size();
    }
    void append(QObject* t)
    {
        int modelIndex = modelOffset() + size();
        beginInsertRows(QModelIndex(), modelIndex, modelIndex);
        QList<QObject*>::append(t);
        endInsertRows();
    }
    void insert(int i, QObject* t)
    {
        int modelIndex = modelOffset() + i;
        beginInsertRows(QModelIndex(), modelIndex, modelIndex);
        QList<QObject*>::insert(i, t);
        endInsertRows();
    }
    void removeLast()
    {
        int modelIndex = modelOffset() + size() - 1;
        beginRemoveRows(QModelIndex(), modelIndex, modelIndex);
        QList<QObject*>::removeLast();
        endRemoveRows();
    }
    void removeAt(int i)
    {
        int modelIndex = modelOffset() + i;
        beginRemoveRows(QModelIndex(), modelIndex, modelIndex);
        QList<QObject*>::removeAt(i);
        endRemoveRows();
    }
private:
    int modelOffset() const { return m_parentTabHistoryItem ? 1 : 0; }
    ParentTabHistoryItem* m_parentTabHistoryItem;
};

#endif
