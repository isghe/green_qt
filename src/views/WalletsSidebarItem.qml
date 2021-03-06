import Blockstream.Green 0.1
import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.13

SidebarItem {
    title: qsTr('WALLETS')

    Repeater {
        model: WalletManager.wallets

        ItemDelegate {
            property Wallet wallet: modelData

            icon.color: 'transparent'
            icon.source: '../' + icons[wallet.network.id]
            icon.width: 32
            icon.height: 32
            text: wallet.name
            width: parent.width

            onClicked: currentWallet = modelData

            Image {
                visible: modelData === currentWallet
                sourceSize.width: 16
                sourceSize.height: 16
                source: '../assets/svg/arrow_right.svg'
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 16
            }
        }
    }
}
