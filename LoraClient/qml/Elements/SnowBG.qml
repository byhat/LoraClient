import QtQuick
import QtQuick.Particles


ParticleSystem {
    id: particleSystem
    anchors.fill: parent

    ImageParticle {
        id: snowflakeParticle
        system: particleSystem
        source: "qrc:/AppQml/presentation/resources/images/snowflake.png"
    }

    Emitter {
        id: snowEmitter
        system: particleSystem
        emitRate: 15
        lifeSpan: 12000
        lifeSpanVariation: 2000
        velocity: PointDirection {
            y: 40
            yVariation: 20
            xVariation: 10
        }
        size: 34
        sizeVariation: 6
        acceleration: PointDirection { y: 5 }

        width: parent.width
        height: 10
        // y: -10
    }
}
