import QtQuick 1.1

Rectangle {
    id: welcomecontainer
    width: 1024
    height: 768

    color: "#272727"

//    gradient: Gradient {
//        GradientStop {
//            position: 0
//            color: "#646262"
//        }

//        GradientStop {
//            position: 1
//            color: "#272727"
//        }
//    }

    Image {
        id: backgroundImage

        fillMode: Image.PreserveAspectFit
        source: "src/qml/welcomepage/images/orca-background.png"

        //anchors.fill: parent
        anchors.bottom: parent.bottom
        anchors.right: parent.right

        // The background OP logo is downscalled to fit the page
        // but not upscalled if page is larger
        property real scale: Math.min(parent.width/sourceSize.width,
                                      parent.height/sourceSize.height,
                                      1.0)

        width: scale*sourceSize.width
        height: scale*sourceSize.height
        smooth: true
    }

    Column {
        id: buttonsLayout
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width

        Row {
            property real availableWidth: welcomecontainer.width
            x: (availableWidth-width)/2
            spacing: 16

            Grid {
                id: buttons
                columns: 3
                spacing: 4
                anchors.verticalCenter: parent.verticalCenter
            }

        }
    }
}
