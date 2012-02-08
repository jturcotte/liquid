/*
    Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
    property bool expanded: false
    id: container
    width: 175

    function focusAndSelect() {
        searchText.focus = true;
        searchText.selectAll();
    }
    function findPrevious() {
        if (!searchText.text)
            focusAndSelect();
        backend.tabManager.currentTab.webView.findPrevious(searchText.text)
    }
    function findNext() {
        if (!searchText.text)
            focusAndSelect();
        backend.tabManager.currentTab.webView.findNext(searchText.text)
    }

    BorderImage {
        id: bg
        source: "pics/card-border.png"
        anchors.fill: parent
        anchors { bottomMargin: -3; topMargin: -3; leftMargin: -3; rightMargin: -3 }
        border { left: 13; top: 13; right: 13; bottom: 13 }
        horizontalTileMode: BorderImage.Repeat
        verticalTileMode: BorderImage.Repeat
    }

    Image {
        id: icon
        opacity: 0
        anchors { left: parent.left; leftMargin: 8; verticalCenter: parent.verticalCenter }
        source: "pics/searchinpage-icon.png"
        MouseArea {
            anchors.fill: parent
            onPressed: {
                expanded = true;
                focusAndSelect();
            }
        }
    }

    Text {
        id: hint
        anchors { left: searchText.left; verticalCenter: parent.verticalCenter }
        text: "Type to find in page"
        color: "#a6a6a6"
        visible: !searchText.text
    }

    TextInput {
        id: searchText
        function find(e) {
            if (e.modifiers & Qt.ShiftModifier)
                findPrevious();
            else
                findNext();
        }
        anchors {
            left: parent.left; right: closeIcon.left
            verticalCenter: parent.verticalCenter
            leftMargin: 5
        }

        font.pixelSize: 14;
        selectByMouse: true
        onTextChanged: backend.tabManager.currentTab.webView.findNext(text)
        Keys.onEnterPressed: find(event)
        Keys.onReturnPressed: find(event)
        Keys.onEscapePressed: {
            expanded = false;
            backend.tabManager.currentTab.webView.focus = true;
        }
        onActiveFocusChanged: if (activeFocus) expanded = true

        // Hack to allow selectAll on enter focus without having mousePress changing the selection after our handler.
        MouseArea {
            anchors.fill: parent
            onPressed: {
                if (!searchText.activeFocus)
                    focusAndSelect();
                else
                    mouse.accepted = false;
            }
        }
    }

    Image {
        id: closeIcon
        anchors { right: parent.right; rightMargin: 2; verticalCenter: parent.verticalCenter }
        source: "pics/close-icon.png"
        MouseArea {
            anchors.fill: parent
            onPressed: {
                expanded = false;
                if (searchText.focus)
                    backend.tabManager.currentTab.webView.focus = true;
            }
        }
    }

    state: "shunk"
    states: [
        State {
            name: "shrunk"
            when: !expanded
            PropertyChanges { target: icon; opacity: 1; anchors.leftMargin: 6 }
            PropertyChanges { target: closeIcon; opacity: 0 }
            PropertyChanges { target: hint; opacity: 0 }
            PropertyChanges { target: container; width: 28 }
            PropertyChanges { target: searchText; opacity: 0 }
        }
    ]
    transitions: Transition {
        NumberAnimation { properties: "width"; easing.type: Easing.OutCubic }
        NumberAnimation { targets: [icon, closeIcon, hint, searchText]; properties: "opacity"; easing.type: Easing.OutCubic }
    }
}
