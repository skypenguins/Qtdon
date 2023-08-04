import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: applicationWindow1
    width: 360
    minimumWidth: 360
    height: 360
    minimumHeight: 360
    title: qsTr("Qtodon")
    visible: true

    Button {
        id: authStartButton
        x: (parent.width - width) / 2
        text: qsTr("Login...")
        onClicked: popup.open()
    }

    Popup {
       id: popup
       contentWidth:  parent.width * 0.8
       contentHeight:  parent.height * 0.4
       x: (parent.width - width) / 2
       y: (parent.height - height) / 2
       modal: true
       focus: true
       closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside


           Frame {
               id: groupBox1
               x: 0
               y: 0
               width: parent.width
               height: hostField.height + 20

               TextField {
                   id: hostField
                   x: 10
                   width: 150
                   placeholderText: qsTr("Type the host name...")
               }

               Button {
                   id: authButton1
                   x: hostField.width + 20
                   text: qsTr("Go to Authentication")
                   onClicked: mastodon.startAuth = hostField.text
               }
           }

           Frame {
               id: groupBox2
               x: 0
               y: groupBox1.height + 10
               width:  parent.width
               height:  authCodeField.height + 20

               TextField {
                   id: authCodeField
                   x: 10
                   width: hostField.width
                   placeholderText: qsTr("Type the Auth Code...")
               }

               Button {
                   id: authButton2
                   x: authCodeField.width + 15
                   text: qsTr("Authorize")
                   onClicked: mastodon.postAuthCode = authCodeField.text
               }
           }


    }

    TextArea {
        id: tootText
        text: ""
        x: 20
        y: 70
        width: 200
        height: 200
        placeholderText: qsTr("Type a toot...")
        anchors.horizontalCenterOffset: 0
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Button {
        id: post
        x: 179
        y: tootText.y + tootText.height + 20
        text: qsTr("TOOT!")
        antialiasing: true
        anchors.horizontalCenterOffset: 1
        anchors.horizontalCenter: parent.horizontalCenter
        transformOrigin: Item.TopLeft
        onClicked: mastodon.toot = tootText.text
    }

}
