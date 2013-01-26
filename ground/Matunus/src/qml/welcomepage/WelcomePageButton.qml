import QtQuick 1.1

Item {
    id: welcomepagebutton
    width: Math.max(100, 100)
    height: 100
    z: 0

    property string baseIconName
    property alias label : labelText.text

    signal clicked

    // Main icon
    Image {
        id: icon
        source: "src/qml/welcomepage/images/orca-abstract.png"
        anchors.centerIn: parent
    }

    Text {
        id: labelText
        anchors.baseline: parent.bottom

        font {
            weight: Font.DemiBold
            pointSize: 14
        }

        color: "#272727"
    }

    // Clickable area
    MouseArea {
        id: mouseArea
        hoverEnabled: true
        anchors.fill: parent

        onClicked: welcomepagebutton.clicked()
    }
}
