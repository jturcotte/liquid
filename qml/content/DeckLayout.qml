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
    property double zoomAmount: 0.0 // From 0.0 to 1.0
    property double zoomPos: 0.5
    property double _totalInMiddle: 0.0
    property int itemOverlap: 0
    property int minIncrementOffset: 0
    property alias model: repeater.model
    property alias delegate: repeater.delegate
    id: main

    function gaussian(x, mu, sigmaSquared) {
        return 1/Math.sqrt(sigmaSquared*2*Math.PI) * Math.exp(-(Math.pow(x - mu, 2)/(2*sigmaSquared)))
    }
    function weightModel(itemCount, itemIndex, panVal, zoomVal) {
        // Multiply the gaussian by 50 to give it a bigger y-span.
        // Add 1 to the gaussian to make sure that base > 1 before pow() then substract one
        // from the result to bring zero back to zero, then add (1 - zoomVal) to progressively flatten the
        // curve as we zoom out. "- 1 + (1 - zoomVal)" is simplified to "- zoomVal".
        var x = itemCount / 2 - 0.5 - itemIndex;
        var mu = ((0.5 - panVal) * itemCount);
        var sigmaSquared = 1;
        return Math.max(0, Math.pow(gaussian(x, mu, sigmaSquared)*50 + 1, zoomVal) - zoomVal);
    }
    function getItems() {
        var items = [];
        for (var i = 0; i < children.length; ++i) {
            // We need a way to tell which children are layouted (e.g. exclude MouseAreas)
            if (!children[i].layouted)
                continue;
            items.push(children[i]);
        }
        return items;
    }
    function redistribute() {
        var items = getItems()

        // Calculate weights and total weight.
        var weigths = [];
        var total = 0;
        var totalBaseWeight = 0;
        for (var i = 0; i < items.length; ++i) {
            var val = weightModel(items.length, i, zoomPos, zoomAmount);
            weigths[i] = val;
            total += val;
            if (items[i].baseWeight)
                totalBaseWeight += items[i].baseWeight;
        }

        // We lose some pixels of width sincewe are using a reference total instead of the real total.
        // This causes the items to move to the left when panning to the left. Here we calculate the lost
        // pixels and translate all items with that amount. There is no need to do anything when panning
        // to the right since those pixels are lost past the last element.
        var lostWidthAdjust = 0;
        if (zoomPos < 0.5)
            lostWidthAdjust = (_totalInMiddle - total) / _totalInMiddle * (width - minIncrementOffset * items.length);

        // Calculate the x pos.
        var curMathX = lostWidthAdjust;
        for (var i = 1; i < items.length; ++i) {
            var mergedRatioFromTotal = weigths[i-1] / _totalInMiddle;
            if (typeof items[i-1].baseWeight !== "undefined")
                mergedRatioFromTotal = zoomAmount * mergedRatioFromTotal + (1 - zoomAmount) * items[i-1].baseWeight / totalBaseWeight;

            // * (width - minIncrementOffset * items.length) + minIncrementOffset;
            // Remove minIncrementOffset from the available width so that we distribute only
            // what is available to, then re-add it for each item.
            curMathX += mergedRatioFromTotal * (width - minIncrementOffset * items.length) + minIncrementOffset;
            items[i].x = curMathX;
        }

        // Remove holes between items starting from the right and then from the left.
        var minRight = width;
        for (var i = items.length - 1; i >= 0 ; --i) {
            if (items[i].x + items[i].width < minRight)
                items[i].x = minRight - items[i].width;
            minRight = items[i].x + itemOverlap;
        }
        var maxLeft = 0;
        for (var i = 0; i < items.length; ++i) {
            if (items[i].x > maxLeft)
                items[i].x = maxLeft;
            maxLeft = items[i].x + items[i].width - itemOverlap;
        }
    }
    function fullRedistribute() {
        // Having an item width calculated using the ratio of it's weight against the total of weights
        // will cause an item to grow if the total is reduced. This is what happens when we pan to the
        // sides of the Gaussian. The remedy to this is to calculate the ratio of the weight to a reference
        // total, which is calculated for all items with a centered Gaussian.
        _totalInMiddle = 0;
        var items = getItems();
        for (var i = 0; i < items.length; ++i)
            _totalInMiddle += weightModel(items.length, i, 0.5, zoomAmount);

        redistribute();
    }

    onWidthChanged: fullRedistribute()
    onZoomPosChanged: redistribute()
    onZoomAmountChanged: fullRedistribute()

    MouseWheelArea {
        anchors.fill: parent

        onWheel: {
            var oldZoomAmount = zoomAmount;
            zoomAmount += delta / 120 * 0.05;
            zoomAmount = Math.max(0.0, Math.min(1.0, zoomAmount));

            // Adjust the position to map the mouse position before and after the zoom.
            var posDistFromWheel = mouseX / width - zoomPos;
            var posAdjust = (posDistFromWheel / zoomAmount - posDistFromWheel * oldZoomAmount / zoomAmount) * zoomAmount;
            // Deals with NaN.
            if (posAdjust)
                zoomPos += posAdjust;
        }
    }

    Repeater {
        id: repeater
        onItemAdded: fullRedistribute()
        onItemRemoved: fullRedistribute()
    }
}
