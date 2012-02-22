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
import liquidext 1.0

Item {
    id: container
    property int itemOverlap: 0
    property int scrollPos: 0
    property alias model: repeater.model
    property alias delegate: repeater.delegate
    property Item rightAlignedItem

    function layout() {
        var items = getItems()
        if (!items.length)
            return

        // The itemOverlap is also substracted from the last item to allow any
        // item anchored to the container to overlap the last item.
        var contentsWidth = items.reduce(function(accum, val) { return accum + val.width - itemOverlap; }, 0)
        scrollPos = Math.max(0, Math.min(contentsWidth - width, scrollPos))

        var curMathX
        // Right align everything if the container is larger than the contents.
        if (contentsWidth < width)
            curMathX = width - contentsWidth
        else
            curMathX = -scrollPos

        for (var i in items) {
            // Add an offset for items (except the first and last) to give
            // a hint to the user that the list can be panned.
            var stackEffectOffset = i > 0 && i < items.length - 1 ? itemOverlap : 0
            items[i].targetX = Math.max(stackEffectOffset, Math.min(width - stackEffectOffset, curMathX))
            curMathX += items[i].width - itemOverlap
        }
    }

    function getItems() {
        var items = []
        for (var i = 0; i < children.length; ++i)
            if (children[i].layouted)
                items.push(children[i])
        return items
    }

    function scrollToRightAlignedItem(alignItem) {
        if (!alignItem || !alignItem.parent)
            return

        // Find the scroll position to place the item on the left, then adjust with width to place it on the right.
        var items = getItems()
        var scrollPosToItem = 0
        var found = false
        for (var i in items) {
            if (items[i] === alignItem) {
                found = true
                break
            }
            scrollPosToItem += items[i].width - itemOverlap
        }
        if (found)
            scrollPos = scrollPosToItem - (width - alignItem.width + itemOverlap)
    }

    onWidthChanged: layout()
    onScrollPosChanged: layout()
    onRightAlignedItemChanged: scrollToRightAlignedItem(rightAlignedItem)

    Repeater {
        id: repeater
        onItemAdded: {
            // rightAlignedItem might have been set before the item was added as child.
            if (item === rightAlignedItem)
                scrollToRightAlignedItem(rightAlignedItem)
            if (item.layouted && item.width)
                layout()
        }
        onItemRemoved: {
            // The item isn't removed yet, make sure it doesn't take space.
            item.layouted = false
            layout()
        }
    }
}
