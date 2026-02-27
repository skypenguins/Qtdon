import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qtdon

ApplicationWindow {
    id: root

    width: 400
    minimumWidth: 360
    height: 500
    minimumHeight: 400
    title: qsTr("Qtdon")
    visible: true

    MastodonClient {
        id: client
    }

    // ── Main content ──────────────────────────────────────────────────

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Button {
            text: client.authenticated
                  ? qsTr("Authenticated ✓")
                  : qsTr("Login...")
            enabled: !client.authenticated
            Layout.alignment: Qt.AlignHCenter
            onClicked: loginPopup.open()
        }

        TextArea {
            id: tootText
            placeholderText: qsTr("What's on your mind?")
            wrapMode: TextEdit.Wrap
            enabled: client.authenticated
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Button {
            text: qsTr("Post")
            enabled: client.authenticated && tootText.text.length > 0
            Layout.alignment: Qt.AlignHCenter
            onClicked: {
                client.postStatus(tootText.text)
                tootText.text = ""
            }
        }

        Label {
            text: client.errorMessage
            color: "red"
            visible: client.errorMessage.length > 0
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }
    }

    // ── Login popup ───────────────────────────────────────────────────

    Popup {
        id: loginPopup
        anchors.centerIn: parent
        width: parent.width * 0.85
        height: 340
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 10

            Label {
                text: qsTr("Login to Mastodon")
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
            }

            TextField {
                id: hostField
                placeholderText: qsTr("Instance (e.g. mastodon.social)")
                Layout.fillWidth: true
            }

            TextField {
                id: clientKeyField
                placeholderText: qsTr("Client key")
                Layout.fillWidth: true
            }

            TextField {
                id: clientSecretField
                placeholderText: qsTr("Client secret")
                echoMode: TextInput.Password
                Layout.fillWidth: true
            }

            TextField {
                id: accessTokenField
                placeholderText: qsTr("Access token (optional)")
                echoMode: TextInput.Password
                Layout.fillWidth: true
            }

            Button {
                text: accessTokenField.text.length > 0
                      ? qsTr("Use Token")
                      : qsTr("Login via Browser")
                Layout.alignment: Qt.AlignHCenter
                onClicked: {
                    if (accessTokenField.text.length > 0) {
                        client.setAccessToken(hostField.text,
                                              accessTokenField.text)
                    } else {
                        client.startAuth(hostField.text,
                                         clientKeyField.text,
                                         clientSecretField.text)
                    }
                }
            }

            Label {
                text: qsTr("Enter an access token to skip browser auth.")
                font.italic: true
                opacity: 0.6
                wrapMode: Text.Wrap
                Layout.fillWidth: true
            }
        }
    }

    // ── Reactive connections ──────────────────────────────────────────

    Connections {
        target: client
        function onAuthenticatedChanged() {
            if (client.authenticated)
                loginPopup.close()
        }
    }
}
