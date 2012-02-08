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

TopTabsLayout {
    id: tabBar

    itemOverlap: 16
    minimizedItemOverlap: 12
    minIncrementOffset: 2
    shrunkWidth: 26
    minimizedWidth: 22
    expandedWidth: 300

    BorderImage {
        id: tabsScrollBar
        property double scrollPos: 0;
        property double size: 1 - tabBar.zoomAmount;

        anchors.bottom: parent.bottom
        height: 5
        visible: size < 1
        source: "pics/scrollbar.png"
        border { left: 15; right: 15; top: 0; bottom: 0 }
        horizontalTileMode: BorderImage.Repeat

        function updateGeometry() {
            width = (parent.width - 50) * size + 50;
            x = (parent.width - width) * scrollPos;
        }
        onSizeChanged: { updateGeometry(); }
        onScrollPosChanged: {
            updateGeometry();
            tabBar.zoomPos = scrollPos;
        }
        MouseArea {
            anchors.fill: parent
            drag.target: tabsScrollBar
            drag.axis: Drag.XAxis
            drag.minimumX: 0
            drag.maximumX: tabsScrollBar.parent.width - tabsScrollBar.width
            onMousePositionChanged: {
                tabsScrollBar.scrollPos = tabsScrollBar.x / (tabsScrollBar.parent.width - tabsScrollBar.width);
            }
        }
    }

    model: backend.tabManager.tabs
    delegate: Item {
        id: tabDelegate
        property bool layouted: visible
        property bool minimized: modelObject.closed
        property double baseWeight: modelObject.baseWeight
        property int tabIndex: modelObject.index
        property int targetX: 0
        property bool inInitialAnim: false;
        width: expandedWidth
        height: parent.height + 2
        x: targetX
        y: backend.tabManager.currentTab == modelObject ? -2 : 0
        z: tabIndex

        Behavior on x { enabled: !inInitialAnim && !inResize; NumberAnimation {} }
        Component.onCompleted: state = minimized ? "minimized" : "";
        onBaseWeightChanged: redistribute();
        onMinimizedChanged: redistribute()

        BorderImage {
            id: borderImage
            anchors.fill: parent
            border { left: 13; top: 13; right: 13; bottom: 0 }
            source: backend.tabManager.currentTab == modelObject ? "pics/tab-border-top.png" : "pics/tab-border-top-disabled.png"
            horizontalTileMode: BorderImage.Repeat
            verticalTileMode: BorderImage.Repeat
        }
        Item {
            id: content
            anchors.fill: parent
            anchors { leftMargin: 4; rightMargin: 4; topMargin: 4 }
            Item {
                id: iconContainer
                width: parent.height
                height: parent.height
                Image {
                    id: icon
                    width: 16
                    height: 16
                    x: (parent.width - width) / 2
                    y: (parent.height - height) / 2
                    source: modelObject.iconSource
                }
            }
            Text {
                height: parent.height
                text: modelObject.title
                verticalAlignment: Text.AlignVCenter
                anchors { left: iconContainer.right; right: parent.right; rightMargin: icon.x }
                elide: Text.ElideRight
            }
        }
        MouseArea {
            anchors.fill: parent
            // FIXME: Use a negative margin on the BorderImage instead to deal with the transparent margin of the image.
            anchors { leftMargin: 3; rightMargin: 3; topMargin: 3; }
            acceptedButtons: Qt.LeftButton | Qt.MiddleButton
            onPressed: {
                if (mouse.button == Qt.LeftButton)
                    backend.tabManager.currentTab = modelObject;
                else if (mouse.button == Qt.MiddleButton)
                    modelObject.close();
            }
        }
        state: "preCreation"
        states: [ State {
            name: "minimized"
            when: minimized
            PropertyChanges { target: tabDelegate; width: minimizedWidth; y: 20; z: 1000 + tabIndex }
        }, State {
            name: "preCreation"
            PropertyChanges { target: tabDelegate; inInitialAnim: true; y: 30 }
        } ]
        transitions: [ Transition {
            to: "minimized"
            reversible: true
            SequentialAnimation {
                NumberAnimation { properties: "width,y"; duration: 250; easing.type: Easing.InOutCubic }
                PropertyAction { property: "z" }
            }
        }, Transition {
            from: "preCreation"
            SequentialAnimation {
                NumberAnimation { property: "y"; duration: 100 }
                PropertyAction { property: "inInitialAnim" }
            }
        } ]
    }
}
