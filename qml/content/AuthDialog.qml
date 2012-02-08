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

Rectangle {
    id: authDialog
    color: "#80000000"

    function show() {
        authDialog.visible = true;
        username.text = backend.machineUsername();
        username.focus = true;
        username.selectAll();
    }
    function done() {
        credentialsEntered(username.text, password.text);
        username.text = "";
        password.text = "";
        authDialog.visible = false;
    }
    function cancel() {
        cancelled();
        username.text = "";
        password.text = "";
        authDialog.visible = false;
    }
    signal credentialsEntered(string username, string password)
    signal cancelled()

    // Eats mouse events on the background.
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton
        onClicked: { cancel(); }
    }

    Rectangle {
        height: 200
        width: 500
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        border.color: "#303030"
        color: "white"

        Column {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            spacing: 20

            Item {
                height: 25
                width: parent.width
                Text {
                    id: labelUsername
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    anchors.verticalCenter: parent.verticalCenter
                    width: 100
                    text: "User name :"
                }
                Rectangle {
                    height: parent.height
                    anchors.left: labelUsername.right
                    anchors.right: parent.right
                    anchors.rightMargin: 20
                    border.color: "#303030"
                    TextInput {
                        id: username
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: 5
                        anchors.rightMargin: 5
                        anchors.verticalCenter: parent.verticalCenter
                        selectByMouse: true
                        KeyNavigation.tab: password
                        KeyNavigation.backtab: username
                        Keys.onEnterPressed: { done(); }
                        Keys.onReturnPressed: { done(); }
                        Keys.onEscapePressed: { cancel(); }
                    }
                }
            }

            Item {
                height: 25
                width: parent.width
                Text {
                    id: labelPassword
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    anchors.verticalCenter: parent.verticalCenter
                    width: 100
                    text: "Password :"
                }
                Rectangle {
                    height: parent.height
                    anchors.left: labelPassword.right
                    anchors.right: parent.right
                    anchors.rightMargin: 20
                    border.color: "#303030"
                    TextInput {
                        id: password
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: 5
                        anchors.rightMargin: 5
                        anchors.verticalCenter: parent.verticalCenter
                        selectByMouse: true
                        echoMode: TextInput.Password
                        KeyNavigation.tab: username
                        KeyNavigation.backtab: username
                        Keys.onEnterPressed: { done(); }
                        Keys.onReturnPressed: { done(); }
                        Keys.onEscapePressed: { cancel(); }
                    }
                }
            }

            Rectangle {
                id: okButton
                anchors.right: parent.right
                anchors.rightMargin: 20
                width: 75
                height: 25
                radius: 5
                border.color: "#303030"
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Ok"
                }
                MouseArea {
                    id: buttonMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: { done(); }
                }
                states: [
                    State {
                        name: "pressed"
                        when: buttonMouseArea.pressedButtons
                        PropertyChanges { target: okButton; border.width: 2 }
                        PropertyChanges { target: okButton; color: "#e0e0e0" }
                    },
                    State {
                        name: "hovered"
                        when: buttonMouseArea.containsMouse
                        PropertyChanges { target: okButton; border.width: 2 }
                    }
                ]
            }
        }
    }
}
