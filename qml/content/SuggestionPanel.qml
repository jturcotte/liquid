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

import QtQuick 1.1

Column {
    property alias model: repeater.model
    property int selectedIndex
    property variant selectedLocation: selectedIndex == -1 ? null : model[selectedIndex]
    property int itemHeight: 28
    signal locationClicked(variant location)

    id: container
    height: itemHeight * model.length

    Repeater {
        id: repeater
        delegate: SuggestionPanelItem {
            width: container.width
            height: itemHeight
            selected: {
                if (index == selectedIndex)
                    return true;
                return false;
            }
            location: modelData
            onClicked: locationClicked(location)
        }
    }
}
