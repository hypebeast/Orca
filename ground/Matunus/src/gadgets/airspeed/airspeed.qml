import QtQuick 1.1

Rectangle {
    id: airspeed
    width: 300
    height: 300
    opacity: 1
    color: "#646464"

    property int maxVelocity: 140
    property int minVelocity: 0
    property int velocity: 0
    property int counter: 0
    property int direction: 0

    function timeChanged() {
        if (counter == 0) {
            direction = 0
        } else if (counter > 140) {
            direction = 1
        }

        if (direction == 0) {
            counter ++
        } else if (direction == 1) {
            counter--
        }

        airspeed.setVelocity(counter)
    }

    Timer {
        interval: 100; running: true; repeat: true;
        onTriggered: airspeed.timeChanged()
    }

    function setVelocity(vel) {
        var step;
        if (vel > 0 && vel < 60) {
            step = 180 / 60
            velocity = step * vel
        } else if (vel > 60 && vel < 140) {
            step = 120 / 80
            velocity = 180 + ((vel - 60)*step)
        } else if (vel >= 140) {
            velocity = 300
        } else if (vel <= 0) {
            velocity = 0
        }
    }

    Image {
        id: background
        source: "src/gadgets/airspeed/images/background.png"
        anchors.fill: parent
        smooth: true
    }

    Image {
        id: needle
        source: "src/gadgets/airspeed/images/needle.png"
        x: 20; y: 100;
        smooth: true
        width: 90
        height: 20
        transform: Rotation {
            id: needleRotation
            origin.x: 90; origin.y: 10;
            angle: airspeed.velocity-90
            Behavior on angle {
                SpringAnimation {
                    spring: 2
                    damping: 0.2
                    modulus: 360
                }
            }
        }
    }
}
