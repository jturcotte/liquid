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
    id: container
    property alias url: inputText.text
    property alias activeFocus: inputText.activeFocus

    signal urlEntered(string url, string enteredText)

    function focusAndSelect() {
        inputText.forceActiveFocus();
        inputText.selectAll();
        suggestionBox.searchOnTop = false;
    }
    function focusAndSelectInSearchMode() {
        inputText.forceActiveFocus();
        inputText.selectAll();
        suggestionBox.searchOnTop = true;
    }

    Location {
        id: webViewLocation
        destination: backend.tabManager.currentTab ? backend.tabManager.currentTab.webView.url : ""
        title: backend.tabManager.currentTab ? backend.tabManager.currentTab.webView.title : ""
        type: Location.WwwType
    }
    property variant manualLocation
    property variant currentLocation: suggestionBox.selectedLocation ? suggestionBox.selectedLocation : (boundToWebViewLocation ? webViewLocation : (!suggestionBox.searchOnTop && backend.historyLocator.hostCompletionLocation ? backend.historyLocator.hostCompletionLocation : manualLocation))
    property bool boundToWebViewLocation: true

    onCurrentLocationChanged: inputText.adjustTextAndSelection()
    Connections {
        target: currentLocation ? currentLocation : null
        onDestinationChanged: inputText.adjustTextAndSelection()
    }
    onBoundToWebViewLocationChanged: {
        if (boundToWebViewLocation)
            suggestionBox.setInputText("");
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

    Rectangle {
        anchors.bottom: parent.bottom
        x: 10; height: 4; color: "#63b1ed"

        width: backend.tabManager.currentTab ? (bg.width - 20) * backend.tabManager.currentTab.webView.progress : 0
        opacity: (!backend.tabManager.currentTab || backend.tabManager.currentTab.webView.progress == 1.0) ? 0.0 : 1.0
    }

    LocationIcon {
        id: icon
        location: currentLocation
        anchors { left: parent.left; leftMargin: 8; verticalCenter: parent.verticalCenter }
        width: Math.max(0, Math.min(sourceSize.width, parent.width - x))
    }

    TextInput {
        id: inputText
        property string enteredText
        property bool textSetGuard: false

        horizontalAlignment: TextEdit.AlignLeft
        selectByMouse: true

        anchors {
            left: icon.right; right: parent.right; leftMargin: 5; rightMargin: 10
            verticalCenter: parent.verticalCenter
        }

        function adjustTextAndSelection() {
            if (boundToWebViewLocation) {
                setText(currentLocation.destination);
                if (activeFocus)
                    selectAll();
                return;
            }

            if (currentLocation == manualLocation) {
                // Make sure we keep the cursor position if the text is the same.
                if (text != currentLocation.inputText)
                    setText(currentLocation.inputText);
                return
            }

            var pos = currentLocation.inputText.search(new RegExp(enteredText, "i"));
            if (pos != -1) {
                setText(currentLocation.inputText.substring(pos));
                select(enteredText.length, text.length);
            } else {
                pos = currentLocation.title.search(new RegExp(enteredText, "i"));
                if (pos != -1) {
                    setText(currentLocation.title.substring(pos));
                    select(enteredText.length, text.length);
                } else
                    // The suggestion doesn't match, show the suggested text and keep our text in enteredText.
                    setText(currentLocation.inputText);
            }
        }

        function setText(newText) {
            textSetGuard = true;
            text = newText;
            textSetGuard = false;
        }

        onTextChanged: {
            // Exit if the text wasn't set by the user.
            if (textSetGuard)
                return;

            // Guess what is the text that the user entered among the displayed text.
            var newEnteredText = text;
            if (selectionEnd == text.length && selectionStart != selectionEnd)
                newEnteredText = text.substring(0, selectionStart);

            // Only update if the new text is not a substring of the previously entered text.
            // This prevents re-asking and getting re-completed when backspacing.
            var updateSuggestions = enteredText.toLowerCase().indexOf(newEnteredText.toLowerCase()) == -1;
            enteredText = newEnteredText;
            if (updateSuggestions)
                suggestionBox.setInputText(newEnteredText);

            // Update the location to go when no suggestion is selected.
            manualLocation = backend.locationFromUserInput(newEnteredText);

            // Text was entered, don't bind to the webview/tab url changes.
            boundToWebViewLocation = false;
        }

        onAccepted: {
            container.urlEntered(currentLocation.destination, enteredText)
            backend.tabManager.currentTab.webView.forceActiveFocus();
        }

        onActiveFocusChanged: {
            if (!activeFocus)
                boundToWebViewLocation = true;
        }

        Keys.onEscapePressed: {
            /*
            The location bar has 3 logical states when it has the focus.
            Pressing escape will go back one step to eventually give the focus back to the web view.

            1. Display the location of the current page.
               - When boundToWebViewLocation == true
               Allows copying and editing the current page url.
            2. Text is entered
               - When not in 1. either 3.
               The typed text is taken as-is as a search or as a URL location.
               If the entered text can be completed as a frequently visited host, it will.
            3. A suggested location is selected from the location bar.
               - When suggestionBox.selectedLocation != null
            */
            if (suggestionBox.selectedLocation)
                suggestionBox.selectNone();
            else if (!boundToWebViewLocation) {
                boundToWebViewLocation = true;
            } else
                backend.tabManager.currentTab.webView.forceActiveFocus();
        }
        Keys.onUpPressed: { suggestionBox.selectPreviousItem() }
        Keys.onDownPressed: suggestionBox.selectNextItem()
        Keys.onTabPressed: suggestionBox.searchOnTop = !suggestionBox.searchOnTop
    }

    // Have an identical Text element just to get the paintedWidth (sigh...)
    Text {
        id: mainText
        anchors { left: inputText.left; top: inputText.top }
        width: inputText.width
        height: inputText.height
        elide: Text.ElideRight
        verticalAlignment: Text.AlignVCenter
        text: inputText.text
        visible: false
    }
    Text {
        id: sideText
        x: mainText.x + mainText.paintedWidth + 2
        y: mainText.y
        width: parent.width - x
        height: mainText.height
        font.pixelSize: 10
        elide: Text.ElideRight
        verticalAlignment: Text.AlignVCenter
        color: currentLocation.type == Location.SearchType ? "#77bbbb" : "#77bb77"
        text: currentLocation.title ? " - " + currentLocation.title : ""
    }

    // Hack to allow selectAll on enter focus without having mousePress changing the selection after our handler.
    MouseArea {
        anchors.fill: parent
        onPressed: {
            if (!inputText.activeFocus)
                focusAndSelect();
            else
                mouse.accepted = false;
        }
    }

    SuggestionBox {
        id: suggestionBox
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.bottom
        anchors.topMargin: 3
        visible: inputText.activeFocus
    }
}
