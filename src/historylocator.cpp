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

#include "historylocator.h"

#include "historydatabase.h"

HistoryLocator::HistoryLocator(HistoryDatabase* database)
    : m_database(database)
    , m_hostCompletionLocation(0)
{
}

void HistoryLocator::setQueryText(const QString& text)
{
    // FIXME: This is ugly.
    qDeleteAll(m_results);
    m_results = m_database->suggestionsForText(text);
    emit resultsChanged();

    if (!text.isEmpty())
        m_hostCompletionLocation.reset(m_database->hostCompletionForText(text));
    else
        m_hostCompletionLocation.reset();
    emit hostCompletionLocationChanged();
}
