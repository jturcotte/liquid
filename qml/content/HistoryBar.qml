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

FanLayout {
    id: container
    property string toolTipText
    property variant toolTipPos
    property bool xBehaviorEnabled: true
    itemOverlap: 3
    // Space for 4 items + adjustments.
    width: (container.height - itemOverlap) * 4 + itemOverlap

    Connections {
        target: backend.tabManager
        onCurrentTabChanged: {
            xBehaviorEnabled = false
            model = backend.tabManager.currentTab ? backend.tabManager.currentTab.history : null
            xBehaviorEnabled = true
        }
    }

    delegate: Item {
        id: historyItem
        property string title: modelObject.title
        property int targetX: container.width
        x: targetX
        Behavior on x { enabled: xBehaviorEnabled; NumberAnimation {} }

        function activate() {
            modelObject.goTo()
        }
        BorderImage {
            id: fancyBorder
            anchors.fill: parent
            anchors { bottomMargin: -3; topMargin: -3; leftMargin: -3; rightMargin: -3 }
            border { left: 13; top: 13; right: 13; bottom: 13 }
            source: modelObject.relativeIndex == 0 ? "pics/card-border.png" : "pics/card-border-inactive.png"
            horizontalTileMode: BorderImage.Repeat
            verticalTileMode: BorderImage.Repeat
        }

        property bool layouted: true
        height: container.height
        width: container.height
        onWidthChanged: layout()

        Item {
            id: iconContainer
            anchors.fill: parent
            anchors { leftMargin: 10; rightMargin: 10; topMargin: 0 }
            Image {
                id: icon
                width: 16
                height: 16
                x: (parent.width - width) / 2
                y: (parent.height - height) / 2
                source: modelObject.iconSource
            }
        }

        Component.onCompleted: {
            if (modelObject.relativeIndex == 0)
                rightAlignedItem = historyItem
        }
    }
    MouseArea {
        property double pressedScrollPos: 0.0
        property variant pressedPos: 0
        property int pressedDistance: 0
        property int lastMoveX: 0
        property Item pressedChild
        // Make sure the mouse area is under the rest so that it doesn't get returned by parent.childAt.
        z: -1
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        hoverEnabled: true
        anchors.fill: container
        onPressed: {
            pressedPos = Qt.point(mouse.x, mouse.y)
            pressedScrollPos = container.scrollPos
            pressedDistance = 0
            lastMoveX = mouse.x
            var child = container.childAt(mouse.x, mouse.y)
            pressedChild = child.layouted ? child : null
        }
        onReleased: {
            if (pressedDistance < 25 && container.childAt(mouse.x, mouse.y) === pressedChild)
                pressedChild.activate()
            toolTipText = ""
        }
        onPositionChanged: {
            toolTipPos = Qt.point(mouse.x, mouse.y)
            if (mouse.buttons) {
                toolTipPos = pressedPos
                pressedDistance += Math.abs(lastMoveX - mouse.x)
                lastMoveX = mouse.x

                xBehaviorEnabled = false
                container.scrollPos = pressedScrollPos - (mouse.x - pressedPos.x)
                xBehaviorEnabled = true
            }
            var hitTest = container.childAt(toolTipPos.x, toolTipPos.y)
            if (hitTest && hitTest.title)
                toolTipText = hitTest.title
            else if (hitTest && hitTest.title === "")
                toolTipText = "[No title]"
            else
                toolTipText = ""
        }
        onExited: toolTipText = ""
    }
}
