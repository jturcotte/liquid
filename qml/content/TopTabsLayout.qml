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
    property int itemOverlap: 0
    property int minimizedItemOverlap: 0
    property int minIncrementOffset: 0
    property int minimizedWidth: 0
    property int shrunkWidth: 0
    property int expandedWidth: 0
    property bool inResize: false
    property alias model: repeater.model
    property alias delegate: repeater.delegate
    id: main

    function layout() {
        var allItems = getAllItems()
        var aliveItems = getAliveItems(allItems)
        if (!aliveItems.length && !allItems.length)
            return

        // === Layout alive items.
        var curShrunkWidth = shrunkWidth
        var curExpandedWidth = Math.min(expandedWidth - itemOverlap, width / 5.0)
        var nextToBeExpandedWidth = shrunkWidth
        var expandedItems = new Array
        var nextToBeExpandedItem
        var minTotalWidth = curShrunkWidth * aliveItems.length
        if (minTotalWidth > width)
            curShrunkWidth = width / aliveItems.length
        else {
            var numExpandedItems = (width - minTotalWidth) / (curExpandedWidth - curShrunkWidth)
            var fraction = numExpandedItems - Math.floor(numExpandedItems)
            numExpandedItems = Math.floor(numExpandedItems)
            // Space is lost if the number is fractional, give this extra space to the next item to be expanded, if any.
            nextToBeExpandedWidth += fraction * (curExpandedWidth - curShrunkWidth)
            // Reverse sort by weight.
            expandedItems = aliveItems.slice(0)
            expandedItems.sort(function(a,b) {return b.baseWeight - a.baseWeight;})
            nextToBeExpandedItem = expandedItems[numExpandedItems]
            expandedItems = expandedItems.slice(0, numExpandedItems)
        }

        // Calculate the x pos.
        var curMathX = 0
        for (var i in aliveItems) {
            aliveItems[i].targetX = curMathX
            curMathX += expandedItems.indexOf(aliveItems[i]) != -1 ? curExpandedWidth : (aliveItems[i] == nextToBeExpandedItem ? nextToBeExpandedWidth : curShrunkWidth)
        }

        // === Layout minimized items.
        // Group adjacent minimized items in subsets.
        var subsets = []
        var currentSubset = {'items': []}
        for (var i in allItems) {
            if (!allItems[i].minimized) {
                if (!currentSubset.items.length)
                    continue
                currentSubset.center = allItems[i].targetX
                subsets.push(currentSubset)
                currentSubset = {'items': []}
            } else {
                currentSubset.items.push(allItems[i])
            }
        }
        if (currentSubset.items.length) {
            currentSubset.center = !aliveItems.length ? 0 : aliveItems[aliveItems.length-1].targetX + aliveItems[aliveItems.length-1].width
            subsets.push(currentSubset)
        }

        for (var i in subsets)
            adjustSubsetToBounds(subsets[i])

        // Check for overlapping subsets in a loop until everything is stable to make sure newly merged subsets don't overlap.
        var lastNumSubsets
        var numSubsets = subsets.length
        do {
            lastNumSubsets = numSubsets
            var lastSubset = null
            for (var i in subsets) {
                var subset = subsets[i]
                if (!lastSubset ||
                    lastSubset.center + lastSubset.items.length * (minimizedWidth - minimizedItemOverlap) / 2.0
                    <= subset.center - subset.items.length * (minimizedWidth - minimizedItemOverlap) / 2.0) {
                    lastSubset = subset
                    continue
                }
                // Calculate the new center and merge the two subsets.
                lastSubset.center = (lastSubset.center * lastSubset.items.length + subset.center * subset.items.length) / (lastSubset.items.length + subset.items.length)
                lastSubset.items = lastSubset.items.concat(subset.items)
                adjustSubsetToBounds(lastSubset)
                delete subsets[i]
            }
            // Count non-undefined values in the array.
            numSubsets = subsets.reduce(function(prev, val) { if (val) return prev + 1; }, 0)
        } while (numSubsets != lastNumSubsets)

        // Calculate the x pos for each minimized item.
        for (var i in subsets) {
            var subset = subsets[i]
            var curX = subset.center - (subset.items.length / 2.0 * (minimizedWidth - minimizedItemOverlap))
            for (var i = 0; i < subset.items.length; ++i) {
                subset.items[i].targetX = curX
                curX += minimizedWidth - minimizedItemOverlap
            }
        }
    }

    function adjustSubsetToBounds(subset) {
        var halfWidth = subset.items.length * (minimizedWidth - minimizedItemOverlap) / 2.0
        if (subset.center - halfWidth < 0)
            subset.center = halfWidth
        else if (subset.center + halfWidth > width)
            subset.center = width - halfWidth
    }
    function getAliveItems(allItems) {
        var items = []
        for (var i in allItems)
            if (!allItems[i].minimized)
                items.push(allItems[i])
        return items
    }
    function getAllItems() {
        var items = []
        for (var i = 0; i < children.length; ++i)
            if (children[i].layouted)
                items.push(children[i])
        // Setting .z on items will mess up the children order, reorder here.
        items.sort(function(a,b) {return a.tabIndex - b.tabIndex;})
        return items
    }

    onWidthChanged: {
        // A behavior on x and stopping it's animation on layout would work better, but it blocks the UI.
        // Use this hack until a better way is found.
        inResize = true
        layout()
        inResize = false
    }

    Repeater {
        id: repeater
        onItemAdded: layout()
        onItemRemoved: {
            // The item isn't removed yet, make sure it doesn't take space.
            item.layouted = false
            layout()
        }
    }
}
