import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0
import Lomiri.Components 1.3

import MediatekRadio 1.0

Page {
		anchors.fill: parent

		header: PageHeader {
			id: header
			title: i18n.tr('Radio Settings')
		}

		ColumnLayout {
			anchors {
				top: header.bottom
                left: parent.left
                right: parent.right
			}

			spacing: units.gu(3)

            RowLayout {
                Layout.topMargin: units.gu(2.5)
                Layout.leftMargin: units.gu(4)
                Layout.rightMargin: units.gu(4)
                Layout.fillWidth: true
    			spacing: units.gu(2)


                Icon {
                    name: "speaker"

                    width: units.gu(2.25)
                    height: units.gu(2.25)

                    color: "#CFCFCF"
                }

                Label {
                    Layout.fillWidth: true
                    text: "Play through loudspeaker"

                    font.pixelSize: units.gu(2.25)

                }

                Switch {
                    checked: true
                }
            }
            
            Rectangle {
                Layout.leftMargin: units.gu(2)
                Layout.rightMargin: units.gu(2)
                Layout.fillWidth: true
                Layout.preferredHeight: units.dp(1)
                color: theme.palette.normal.base
            }
        }
}