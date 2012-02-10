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

import "content"
import "content/ShortcutHandler.js" as ShortcutHandler

Rectangle {
    function addNewTab(url) {
        backend.tabManager.currentTab = backend.tabManager.addNewTab(0, url)
    }
    function addNewEmptyTab() {
        backend.tabManager.currentTab = backend.tabManager.addNewTab()
        locationBar.focusAndSelect();
    }

    width: 800; height: 600
    color: "transparent"

    Item {
        anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }
        Item {
            id: webView
            anchors.fill: parent
            property variant tabMonitor: backend.tabManager.currentTab
            onTabMonitorChanged: {
                var oldView = children[0];
                if (oldView) {
                    oldView.visible = false;
                    oldView.parent = null;
                }
                if (tabMonitor) {
                    tabMonitor.webView.visible = true;
                    tabMonitor.webView.parent = webView;
                    tabMonitor.webView.anchors.fill = webView;
                    tabMonitor.webView.forceActiveFocus();
                    ShortcutHandler.setHandler("GoBack", tabMonitor.webView.back.trigger);
                    ShortcutHandler.setHandler("GoForward", tabMonitor.webView.forward.trigger);
                    ShortcutHandler.setHandler("Reload", tabMonitor.webView.reload.trigger);
                    ShortcutHandler.setHandler("Stop", tabMonitor.webView.stop.trigger);
                    ShortcutHandler.setHandler("CloseTab", tabMonitor.close);
                }
            }
        }
        Image {
            id: top
            anchors { top: parent.top; left: parent.left; right: parent.right }
            height: sourceSize.height
            fillMode: Image.Tile
            source: "content/pics/drop-shadow-top.png"
        }

        MouseArea {
            anchors.fill: parent
            onPressed: {
                backend.tabManager.currentTab.webView.forceActiveFocus();
                mouse.accepted = false;
            }
        }
    }

    Rectangle {
        id: header
        anchors { top: parent.top; left: parent.left; right: parent.right }

        width: 800
        height: column.height
        color: "transparent"

        Column {
            id: column
            width: parent.width

            Item {
                width: parent.width
                height: 28
                Item {
                    // This item will clip the TabBar items to disapear under the new tab button.
                    anchors { top: parent.top; bottom: parent.bottom; left: parent.left; right: newTabButton.left }
                    anchors.rightMargin: -(newTabButton.width-10)
                    clip: true
                    TabBar {
                        id: tabBar
                        anchors.fill: parent
                        anchors.rightMargin: -parent.anchors.rightMargin
                    }
                }
                Item {
                    id: newTabButton
                    width: 35
                    anchors { top: parent.top; bottom: parent.bottom; right: parent.right }
                    BorderImage {
                        anchors.fill: parent
                        anchors.topMargin: -2
                        border { left: 13; top: 13; right: 13; bottom: 0 }
                        source: "content/pics/tab-border-top.png"
                        horizontalTileMode: BorderImage.Repeat
                        verticalTileMode: BorderImage.Repeat
                    }
                    Image {
                        width: 16
                        height: 16
                        source: "content/pics/add-tab-button.png"
                        anchors.centerIn: parent
                        anchors.verticalCenterOffset: 2
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: addNewEmptyTab()
                    }
                }
                Image {
                    anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
                    height: sourceSize.height
                    fillMode: Image.Tile
                    source: "content/pics/drop-shadow-bottom.png"
                }
            }

            Rectangle {
                width: parent.width; height: 40
                color: "white"

                HistoryBar {
                    id: historyBar
                    anchors { top: parent.top; bottom: parent.bottom; left: parent.left; leftMargin: 3; topMargin: 6; bottomMargin: 6 }
                    width: 150
                }

                LocationBar {
                    id: locationBar
                    url: backend.tabManager.currentTab ? backend.tabManager.currentTab.webView.url : ""
                    anchors {
                        left: historyBar.right; right: searchInPage.left; top: parent.top; bottom: parent.bottom
                        bottomMargin: 5; topMargin: 5; rightMargin: 3
                    }
                    onUrlEntered: {
                        backend.tabManager.currentTab.loadUrl(url, enteredText);
                        backend.tabManager.currentTab.webView.forceActiveFocus();
                    }
                }

                SearchInPage {
                    id: searchInPage
                    anchors {
                        right: parent.right; top: parent.top; bottom: parent.bottom
                        bottomMargin: 5; topMargin: 5; rightMargin: 3
                    }
                }
            }
        }
    }

    ToolTip {
        sourceItem: tabBar.toolTipText ? tabBar
            : historyBar.toolTipText ? historyBar
            : backend.tabManager.currentTab && backend.tabManager.currentTab.webView.toolTipText ? backend.tabManager.currentTab.webView
            : null
    }

    AuthDialog {
        id: authDialog
        anchors.fill: parent
        visible: false
        Connections {
            target: backend
            onCredentialsNeeded: {
                authDialog.show();
            }
        }
        onCredentialsEntered: {
            backend.enterCredentials(username, password);
        }
        onCancelled: {
            backend.cancelAuth();
        }
    }

    Keys.onPressed: {
        ShortcutHandler.handleEvent(event);
    }
    Component.onCompleted: {
        ShortcutHandler.setHandler("OpenNewTab", addNewEmptyTab);
        ShortcutHandler.setHandler("ShowNextTab", backend.tabManager.showNextTab);
        ShortcutHandler.setHandler("ShowPreviousTab", backend.tabManager.showPreviousTab);
        ShortcutHandler.setHandler("FocusLocationBar_HistoryMode", locationBar.focusAndSelect);
        ShortcutHandler.setHandler("FocusLocationBar_SearchMode", locationBar.focusAndSelectInSearchMode);
        ShortcutHandler.setHandler("FocusSearchInPage", searchInPage.focusAndSelect);
        ShortcutHandler.setHandler("FindPreviousInPage", searchInPage.findPrevious);
        ShortcutHandler.setHandler("FindNextInPage", searchInPage.findNext);
    }
}
