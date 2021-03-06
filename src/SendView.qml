import Blockstream.Green 0.1
import QtMultimedia 5.13
import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import './views'

StackView {
    id: stack_view
    property alias address: address_field.address
    property alias amount: amount_field.amount
    property alias sendAll: send_all_button.checked

    property var actions: currentItem.actions

    states: State {
        name: 'SCAN_QR_CODE'
        PropertyChanges {
            target: camera
            cameraState: Camera.ActiveState
        }
    }

    transitions: [
        Transition {
            to: 'SCAN_QR_CODE'
            StackViewPushAction {
                stackView: stack_view
                ScannerView {
                    property list<Action> actions: [Action{
                            text: qsTr('id_cancel')
                            onTriggered: stack_view.state = ''
                       }]

                    source: camera

                    onCodeScanned: {
                        address_field.address = WalletManager.parseUrl(code).address
                        stack_view.state = ''
                    }
                }
            }
        },
        Transition {
            from: 'SCAN_QR_CODE'
            ScriptAction {
                script: stack_view.pop()
            }
        }
    ]

    property Camera camera: Camera {
        cameraState: Camera.LoadedState
        focus {
            focusMode: CameraFocus.FocusContinuous
            focusPointMode: CameraFocus.FocusPointAuto
        }
    }

    initialItem: ColumnLayout {

        property list<Action> actions: [Action{
               text: qsTr('id_send')
               enabled: controller.valid && !controller.transaction.error
               onTriggered: controller.send()
           }]

        spacing: 16
        //anchors.fill: parent

        Label {
            text: controller.transaction.error || ''
        }

        AddressField {
            id: address_field
            Layout.fillWidth: true
            label: qsTr("id_recipient")

            onOpenScanner: send_view.state = 'SCAN_QR_CODE'
        }

        AmountField {
            id: amount_field
            Layout.fillWidth: true
            currency: 'BTC'
            label: qsTr('id_amount')
            enabled: !send_all_button.checked

            Binding on amount {
                when: send_all_button.checked
                value: account.balance
            }
        }

        FlatButton {
            id: send_all_button
            checkable: true
            text: qsTr('id_send_all_funds')
        }

        Label {
            text: qsTr('id_network_fee')
        }

        FeeComboBox {
            Layout.fillWidth: true
            property var indexes: [3, 12, 24]

            Component.onCompleted: {
                currentIndex = indexes.indexOf(account.wallet.settings.required_num_blocks)
                controller.feeRate = account.wallet.events.fees[blocks]
            }

            onBlocksChanged: {
                console.log('blocks: ', blocks)
                controller.feeRate = account.wallet.events.fees[blocks]
            }
        }
    }
}
