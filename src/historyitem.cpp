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

#include "historyitem.h"

HistoryItem::HistoryItem(int qWebHistoryIndex, Tab* tab, QObject* parent)
    : QObject(parent)
    , m_qWebHistoryIndex(qWebHistoryIndex)
    , m_tab(tab)
{
}

void HistoryItem::goTo()
{
    m_tab->webView()->history()->goToItem(m_tab->webView()->history()->itemAt(m_qWebHistoryIndex));
}

QUrl HistoryItem::iconSource()
{
    QUrl url = m_tab->webView()->history()->itemAt(m_qWebHistoryIndex).url();
    return (!QWebSettings::iconForUrl(url).isNull()) ? QUrl("image://tabs/" + url.toEncoded()) : QUrl("image://tabs/defaultIcon");
}

QString HistoryItem::title()
{
    return m_tab->webView()->history()->itemAt(m_qWebHistoryIndex).title();
}

void HistoryItem::checkIcon()
{
    emit iconSourceChanged();
}

void HistoryItem::checkRelativeIndex()
{
    emit relativeIndexChanged();
    emit titleChanged();
}

int HistoryItem::relativeIndex() const
{
    return m_tab->webView()->history()->currentItemIndex() - m_qWebHistoryIndex;
}
