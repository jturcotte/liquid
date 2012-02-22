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

Item {
    id: container
    property bool searchOnTop: false
    property variant selectedLocation: historySuggestions.selectedLocation ? historySuggestions.selectedLocation : searchSuggestions.selectedLocation
    signal locationClicked(variant location)

    function selectPreviousItem() {
        --_selectedIndex
        if (_selectedIndex <= -1)
            _selectedIndex = -1
    }
    function selectNextItem() {
        ++_selectedIndex
        if (_selectedIndex >= historySuggestions.model.length + searchSuggestions.model.length)
            _selectedIndex = historySuggestions.model.length + searchSuggestions.model.length - 1
    }
    function selectNone() {
        _selectedIndex = -1
    }
    function setInputText(text) {
        backend.historyLocator.setQueryText(text)
        backend.searchLocator.setQueryText(text)
        _selectedIndex = -1
    }

    function selectedIndexForPanel(panel) {
        if (_panelOnTop != panel)
            return Math.max(-1, _selectedIndex - _panelOnTop.model.length)
        return _selectedIndex
    }
    property int _selectedIndex: -1
    property Item _panelOnTop: searchOnTop ? searchSuggestions : historySuggestions
    height: 2 + historySuggestions.height + searchSuggestions.height

    BorderImage {
        source: "pics/card-border.png"
        anchors.fill: parent
        anchors { bottomMargin: -3; topMargin: -3; leftMargin: -3; rightMargin: -3 }
        border { left: 13; top: 13; right: 13; bottom: 13 }
        horizontalTileMode: BorderImage.Repeat
        verticalTileMode: BorderImage.Repeat
    }

    Item {
        id: contentsBox
        anchors { fill: parent; topMargin: 1; leftMargin: 1; rightMargin: 1; bottomMargin: 1 }
        SuggestionPanel {
            id: historySuggestions
            model: backend.historyLocator.results
            selectedIndex: selectedIndexForPanel(historySuggestions)
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            onLocationClicked: container.locationClicked(location)
        }
        SuggestionPanel {
            id: searchSuggestions
            model: backend.searchLocator.results
            selectedIndex: selectedIndexForPanel(searchSuggestions)
            anchors.top: historySuggestions.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            onLocationClicked: container.locationClicked(location)
        }
    }

    states: State {
        name: "searchOnTop"
        when: _panelOnTop == searchSuggestions
        AnchorChanges { target: historySuggestions; anchors.top: searchSuggestions.bottom }
        AnchorChanges { target: searchSuggestions; anchors.top: parent.top }
    }

    transitions: [ Transition {
        to: "searchOnTop"
        ParallelAnimation {
            SequentialAnimation {
                // Set clipping only during transitions since children can be drawn before the container's height is set correctly.
                PropertyAction { target: contentsBox; property: "clip"; value: true}
                NumberAnimation { target: searchSuggestions; property: "y"; to: contentsBox.height - searchSuggestions.height / 2; duration: 75 }
                NumberAnimation { target: searchSuggestions; property: "y"; from: -searchSuggestions.height / 2; to: 0; duration: 75 }
                PropertyAction { target: contentsBox; property: "clip"; value: false}
            }
            AnchorAnimation { targets: historySuggestions; duration: 150 }
        }
    }, Transition {
        from: "searchOnTop"
        ParallelAnimation {
            SequentialAnimation {
                PropertyAction { target: contentsBox; property: "clip"; value: true}
                NumberAnimation { target: searchSuggestions; property: "y"; to: -searchSuggestions.height / 2; duration: 75 }
                NumberAnimation { target: searchSuggestions; property: "y"; from: contentsBox.height - searchSuggestions.height / 2; to: historySuggestions.height; duration: 75 }
                PropertyAction { target: contentsBox; property: "clip"; value: false}
            }
            AnchorAnimation { targets: historySuggestions; duration: 150 }
        }
    } ]

    Component.onCompleted: {
        backend.historyLocator.setQueryText("")
        backend.searchLocator.setQueryText("")
    }
}

