import Lomiri.Components 1.3
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0

import MediatekRadio 1.0

Page {
		anchors.fill: parent

		header: PageHeader {
			id: header
			title: i18n.tr('FM Radio')
			trailingActionBar {
				numberOfSlots: 3

				actions: [
					Action {
						iconName: "settings"
                        onTriggered: {
                            mainStack.push(Qt.resolvedUrl("SettingsPage.qml"))
                        }
					},
					Action {
						iconName: "view-list-symbolic"
					},
					Action {
						iconName: "toolkit_input-search"
					}
				]
			}
		}

		ColumnLayout {
			anchors {
				top: header.bottom
			}
			anchors.horizontalCenter: parent.horizontalCenter

			spacing: units.gu(3)

			// Button {
			// 	text: i18n.tr('\<')
			// 	onClicked: {
			// 		MediatekRadio.seekDown()
			// 		frequency.text = MediatekRadio.getFrequency() / 100
			// 	}
			// }

			// Button {
			// 	text: i18n.tr('\>')
			// 	onClicked: {
			// 		MediatekRadio.seekUp()
			// 		frequency.text = MediatekRadio.getFrequency() / 100
			// 	}
			// }

			// Frequency
			RowLayout {
				Layout.topMargin: units.gu(8)
				Layout.alignment: Qt.AlignHCenter
				spacing: units.gu(1)

				Label {
					id: frequency
					Layout.alignment: Qt.AlignHCenter
					font.pixelSize: units.gu(10)
					text: MediatekRadio.getFrequency() / 100
				}

				Label {
					font.pixelSize: units.gu(5)
					font.weight: Font.light
					text: "MHz"
					color: LomiriColors.slate

					Layout.alignment: Qt.AlignBottom
					Layout.bottomMargin: units.gu(1)
				}
			}

			// Playback buttons
			RowLayout {
				Layout.topMargin: units.gu(2)
				Layout.alignment: Qt.AlignHCenter
				spacing: units.gu(4)

				Button {
					id: previousButton

					Layout.alignment: Qt.AlignHCenter
					Layout.preferredWidth: units.gu(6)
					Layout.preferredHeight: units.gu(6)

					contentItem: Item {
						Icon {
							anchors.centerIn: parent

							name: "media-skip-backward"

							width: units.gu(3)
							height: units.gu(3)

							color: "white"
						}
					}

					background: Rectangle {
						radius: width / 2
						color: "#2A2A2A"

						border.width: units.dp(1)
						border.color: '#50ffffff'
					}

					onClicked: {
						// Previous station in the 4 shortcuts
					}
				}

				// Button {
				// 	// id: previousButton

				// 	Layout.alignment: Qt.AlignHCenter
				// 	Layout.preferredWidth: units.gu(7)
				// 	Layout.preferredHeight: units.gu(7)

				// 	contentItem: Item {
				// 		Icon {
				// 			anchors.centerIn: parent

				// 			name: "go-previous"

				// 			width: units.gu(3.5)
				// 			height: units.gu(3.5)

				// 			color: "white"
				// 		}
				// 	}

				// 	background: Rectangle {
				// 		radius: width / 2
				// 		color: "#2A2A2A"

				// 		border.width: units.dp(1)
				// 		border.color: '#50ffffff'
				// 	}

				// 	onClicked: {
				// 		// Previous station in the 4 shortcuts
				// 	}
				// }

				Button {
					id: startStopButton

					Layout.alignment: Qt.AlignHCenter
					Layout.preferredWidth: units.gu(10)
					Layout.preferredHeight: units.gu(10)

					contentItem: Item {
						Icon {
							anchors.centerIn: parent

							name: root.radioRunning
								? "media-playback-pause"
								: "media-playback-start"

							width: units.gu(5)
							height: units.gu(5)

							color: "white"
						}
					}

					background: Rectangle {
						id: startStopButtonBg
						radius: width / 2
						color: "#2A2A2A"

						border.width: units.dp(1)
						border.color: '#50ffffff'
					}

					onClicked: {
						if (!root.radioRunning) {
							MediatekRadio.startRadio(MediatekRadio.getFrequency())
						} else {
							MediatekRadio.stopRadio()
						}

						root.radioRunning = MediatekRadio.isRadioRunning()
					}
				}

				// Button {
				// 	// id: forwardButton

				// 	Layout.alignment: Qt.AlignHCenter
				// 	Layout.preferredWidth: units.gu(7)
				// 	Layout.preferredHeight: units.gu(7)

				// 	contentItem: Item {
				// 		Icon {
				// 			anchors.centerIn: parent

				// 			name: "go-next"

				// 			width: units.gu(3.5)
				// 			height: units.gu(3.5)

				// 			color: "white"
				// 		}
				// 	}

				// 	background: Rectangle {
				// 		radius: width / 2
				// 		color: "#2A2A2A"

				// 		border.width: units.dp(1)
				// 		border.color: '#50ffffff'
				// 	}

				// 	onClicked: {
				// 		if (!root.radioRunning) {
				// 			MediatekRadio.startRadio(MediatekRadio.getFrequency())
				// 		} else {
				// 			MediatekRadio.stopRadio()
				// 		}

				// 		root.radioRunning = MediatekRadio.isRadioRunning()
				// 	}
				// }

				Button {
					id: forwardButton

					Layout.alignment: Qt.AlignHCenter
					Layout.preferredWidth: units.gu(6)
					Layout.preferredHeight: units.gu(6)

					contentItem: Item {
						Icon {
							anchors.centerIn: parent

							name: "media-skip-forward"

							width: units.gu(3)
							height: units.gu(3)

							color: "white"
						}
					}

					background: Rectangle {
						radius: width / 2
						color: "#2A2A2A"

						border.width: units.dp(1)
						border.color: '#50ffffff'
					}

					onClicked: {
						if (!root.radioRunning) {
							MediatekRadio.startRadio(MediatekRadio.getFrequency())
						} else {
							MediatekRadio.stopRadio()
						}

						root.radioRunning = MediatekRadio.isRadioRunning()
					}
				}
			}

			// Record button, Add/Remove 0.1
			RowLayout {
				Layout.topMargin: units.gu(4)
				Layout.alignment: Qt.AlignHCenter
				spacing: units.gu(2)

				Button {
					// id: previousButton

					Layout.alignment: Qt.AlignHCenter

					contentItem: Row {
						spacing: units.gu(0.5)

						Icon {
							name: "list-remove"

							width: units.gu(2)
							height: units.gu(2)

							color: "white"
						}

						Label {
							text: "0.1"
						}
					}

					background: Rectangle {
						radius: width / 2
						color: "#2A2A2A"

						border.width: units.dp(1)
						border.color: '#50ffffff'
					}

					onClicked: {
						MediatekRadio.tune(MediatekRadio.getFrequency() - (0.1 * 100))
						frequency.text = MediatekRadio.getFrequency() / 100
					}
				}

				Button {
					// id: previousButton

					Layout.alignment: Qt.AlignHCenter

					contentItem: Row {
						spacing: units.gu(0.5)

						Icon {
							name: "media-record"

							width: units.gu(2)
							height: units.gu(2)
						}

						Label {
							text: "Record"
						}
					}

					background: Rectangle {
						radius: width / 2
						color: "#2A2A2A"

						border.width: units.dp(1)
						border.color: '#50ffffff'
					}

					onClicked: {
						// Previous station in the 4 shortcuts
					}
				}

				Button {
					// id: previousButton

					Layout.alignment: Qt.AlignHCenter

					contentItem: Row {
						spacing: units.gu(0.5)

						Label {
							text: "0.1"
						}

						Icon {
							name: "list-add"

							width: units.gu(2)
							height: units.gu(2)

							color: "white"
						}
					}

					background: Rectangle {
						radius: width / 2
						color: "#2A2A2A"

						border.width: units.dp(1)
						border.color: '#50ffffff'
					}

					onClicked: {
						MediatekRadio.tune(MediatekRadio.getFrequency() + (0.1 * 100))
						frequency.text = MediatekRadio.getFrequency() / 100
					}
				}
			}

			// 4 Pinned Stations
			RowLayout {
				Layout.alignment: Qt.AlignHCenter
				spacing: units.gu(2)

				Button {
					text: i18n.tr('98.5')
					background: Rectangle {
						radius: width / 2
						color: "#2A2A2A"

						border.width: units.dp(1)
						border.color: '#50ffffff'
					}

					onClicked: {
						
					}
				}
				Button {
					text: i18n.tr('101.1')
					background: Rectangle {
						radius: width / 2
						color: "#181818"
					}

					onClicked: {
						
					}
				}
				Button {
					text: i18n.tr('102.5')
					background: Rectangle {
						radius: width / 2
						color: "#181818"
					}

					onClicked: {
						
					}
				}
				Button {
					contentItem: Row {
						spacing: units.gu(0.5)

						Icon {
							name: "unpinned"

							width: units.gu(2)
							height: units.gu(2)

							color: "white"
						}

						Label {
							text: "Pin"
						}
					}
					background: Rectangle {
						radius: width / 2
						color: "#181818"
					}

					onClicked: {
						
					}
				}
			}
		}

		BottomEdge {
			id: bottomEdge
			width: units.gu(45)
			height: units.gu(45)

			contentComponent: TextInput {
				id: frequencyInput
				font.pixelSize: units.gu(5)
				inputMask: "990.9;_"
				inputMethodHints: Qt.ImhFormattedNumbersOnly
				text: MediatekRadio.getFrequency() / 100
				onEditingFinished: {
					MediatekRadio.tune(frequencyInput.text * 100)
					frequency.text = MediatekRadio.getFrequency() / 100
					bottomEdge.collapse()
				}

				Connections {
					target: bottomEdge
					onCommitCompleted: {
						focus = true
					}
				}

				Connections {
					target: bottomEdge
					onCollapseCompleted: {
						focus = false
					}
				}


			}

		}

	}