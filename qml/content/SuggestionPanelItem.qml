import QtQuick 1.1
import liquidext 1.0

Item {
    id: container
    property variant location
    property bool selected

    Rectangle {
        anchors.fill: parent
        color: "lightgreen"
        visible: selected
    }
    LocationIcon {
        id: icon
        location: container.location
        anchors { left: parent.left; leftMargin: 7; verticalCenter: parent.verticalCenter }
        width: Math.max(0, Math.min(sourceSize.width, parent.width - x))
        opacity: 0.40
    }
    Text {
        id: mainText
        anchors { left: icon.right; right: parent.right; leftMargin: 5 }
        height: parent.height
        elide: Text.ElideRight
        verticalAlignment: Text.AlignVCenter
        color: "gray"
        text: {
            if (location.type == Location.SearchType)
                location.inputText
            else if (location.title)
                location.title
            else
                location.destination
        }
    }
    Text {
        property string displayedText: location.type == Location.SearchType ? location.title : location.destination
        id: sideText
        x: mainText.x + mainText.paintedWidth + 3
        width: parent.width - x
        height: parent.height
        font.pixelSize: 10
        elide: Text.ElideRight
        verticalAlignment: Text.AlignVCenter
        color: location.type == Location.SearchType ? "#77bbbb" : "#77bb77"
        visible: displayedText
        text: "- " + displayedText
    }
}
