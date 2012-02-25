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

#include "tabhistorylistmodel.h"

#include "historyitem.h"

void TabHistoryListModel::setParentTabHistoryItem(ParentTabHistoryItem* item)
{
    if (item && !m_parentTabHistoryItem) {
        beginInsertRows(QModelIndex(), 0, 0);
        m_parentTabHistoryItem = item;
        endInsertRows();
    } else if (!item && m_parentTabHistoryItem) {
        beginRemoveRows(QModelIndex(), 0, 0);
        m_parentTabHistoryItem = 0;
        endRemoveRows();
    } else if (item != m_parentTabHistoryItem) {
        m_parentTabHistoryItem = item;
        QModelIndex index = createIndex(0, 0);
        emit dataChanged(index, index);
    }
}
QVariant TabHistoryListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::UserRole)
        return QVariant();
    if (m_parentTabHistoryItem && index.row() == 0)
        return QVariant::fromValue<QObject*>(m_parentTabHistoryItem);
    int i = index.row() - modelOffset();
    return QVariant::fromValue(at(i));
}

