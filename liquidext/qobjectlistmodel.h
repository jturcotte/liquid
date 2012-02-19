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

#ifndef QOBJECTLISTMODEL_H
#define QOBJECTLISTMODEL_H

#include <QAbstractListModel>

class QObjectListModel : public QAbstractListModel, public QList<QObject*> {
    Q_OBJECT
public:
    QObjectListModel()
    {
        QHash<int, QByteArray> roles;
        roles[Qt::UserRole] = "modelObject";
        setRoleNames(roles);
    }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
    {
        if (!index.isValid() || role != Qt::UserRole)
            return QVariant();
        return QVariant::fromValue(at(index.row()));
    }
    int rowCount(const QModelIndex&) const
    {
        return size();
    }
    void append(QObject* t)
    {
        beginInsertRows(QModelIndex(), size(), size());
        QList<QObject*>::append(t);
        endInsertRows();
    }
    void insert(int i, QObject* t)
    {
        beginInsertRows(QModelIndex(), i, i);
        QList<QObject*>::insert(i, t);
        endInsertRows();
    }
    void removeAt(int i)
    {
        beginRemoveRows(QModelIndex(), i, i);
        QList<QObject*>::removeAt(i);
        endRemoveRows();
    }
};

#endif
