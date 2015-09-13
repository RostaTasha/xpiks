/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2015 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.1
import QtQuick.Controls.Styles 1.1
import xpiks 1.0
import "../Constants"
import "../Constants/Colors.js" as Colors;
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"

Item {
    id: dialogComponent
    anchors.fill: parent

    signal onDialogDestruction();
    Component.onDestruction: onDialogDestruction();

    function closePopup() {
        dialogComponent.destroy()
    }

    function mustUseConfirmation() {
        return appSettings.boolValue(appSettings.useConfirmationDialogsKey, true)
    }

    PropertyAnimation { target: dialogComponent; property: "opacity";
        duration: 400; from: 0; to: 1;
        easing.type: Easing.InOutQuad ; running: true }

    MessageDialog {
        id: confirmRemoveArtworksDialog
        property int itemsCount
        title: "Confirmation"
        text: qsTr("Are you sure you want to remove %1 item(s)?").arg(itemsCount)
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            doRemoveSelectedArtworks()
        }
    }

    function doRemoveSelectedArtworks() {
        combinedArtworks.removeSelectedArtworks()
        if (combinedArtworks.artworksCount === 0) {
            closePopup()
        }
    }

    // This rectange is the a overlay to partially show the parent through it
    // and clicking outside of the 'dialog' popup will do 'nothing'
    Rectangle {
        anchors.fill: parent
        id: overlay
        color: "#000000"
        opacity: 0.6
        // add a mouse area so that clicks outside
        // the dialog window will not do anything
        MouseArea {
            anchors.fill: parent
        }
    }

    FocusScope {
        anchors.fill: parent

        MouseArea {
            anchors.fill: parent
            onWheel: wheel.accepted = true
            onClicked: mouse.accepted = true

            property real old_x : 0
            property real old_y : 0

            onPressed:{
                var tmp = mapToItem(dialogComponent, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(dialogComponent, dialogWindow, mouse, old_x, old_y);
                old_x = old_xy[0]; old_y = old_xy[1];
            }
        }

        // This rectangle is the actual popup
        Rectangle {
            id: dialogWindow
            width: 730
            height: 550
            color: Colors.selectedArtworkColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 30
                spacing: 3

                RowLayout {
                    Layout.fillWidth: true

                    StyledText {
                        text: qsTr("Edit multiple artworks")
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledText {
                        text: combinedArtworks.artworksCount == 1 ? qsTr("1 artwork begin edited") : qsTr("%1 artworks being editing").arg(combinedArtworks.artworksCount)
                        color: Colors.defaultInputBackground
                    }
                }

                Item {
                    height: 10
                }

                RowLayout {
                    spacing: 5
                    Layout.fillWidth: true
                    height: 40

                    StyledText {
                        height: parent.height
                        text: qsTr("Select to remove")
                        verticalAlignment: Text.AlignBottom
                        color: Colors.defaultInputBackground
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        text: qsTr("Remove")
                        width: 100
                        enabled: combinedArtworks.selectedArtworksCount > 0
                        onClicked: {
                            if (mustUseConfirmation()) {
                                confirmRemoveArtworksDialog.itemsCount = combinedArtworks.selectedArtworksCount
                                confirmRemoveArtworksDialog.open()
                            } else {
                                doRemoveSelectedArtworks()
                            }
                        }
                    }
                }

                Item {
                    height: 1
                }

                Rectangle {
                    height: 130
                    width: parent.width
                    color: Colors.defaultControlColor

                    StyledScrollView {
                        id: imagesScrollView
                        height: parent.height + 15
                        width: parent.width
                        anchors.margins: 10

                        ListView {
                            boundsBehavior: Flickable.StopAtBounds
                            anchors.fill: parent
                            anchors.margins: 10
                            orientation: Qt.Horizontal
                            spacing: 10
                            model: combinedArtworks

                            add: Transition {
                                NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 230 }
                            }

                            remove: Transition {
                                NumberAnimation { property: "opacity"; to: 0; duration: 230 }
                            }

                            displaced: Transition {
                                NumberAnimation { properties: "x,y"; duration: 230 }
                            }

                            addDisplaced: Transition {
                                NumberAnimation { properties: "x,y"; duration: 230 }
                            }

                            removeDisplaced: Transition {
                                NumberAnimation { properties: "x,y"; duration: 230 }
                            }

                            delegate: Rectangle {
                                property int delegateIndex: index
                                id: imageWrapper
                                height: 110
                                width: height
                                color: "transparent"

                                Image {
                                    anchors.fill: parent
                                    anchors.margins: 1
                                    source: "image://global/" + path
                                    sourceSize.width: 150
                                    sourceSize.height: 150
                                    fillMode: Image.PreserveAspectCrop
                                    asynchronous: true
                                }

                                Rectangle {
                                    anchors.fill: parent
                                    color: Colors.defaultControlColor
                                    opacity: isselected ? (mouseArea.containsMouse ? 0.6 : 0.7) : (mouseArea.containsMouse ? 0.4 : 0)
                                }

                                LargeRemoveIcon {
                                    opacity: isselected ? (mouseArea.containsMouse ? 0.85 : 1) : (mouseArea.containsMouse ? 0.6 : 0)
                                    width: parent.width
                                    height: parent.height
                                }

                                MouseArea {
                                    id: mouseArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onClicked: {
                                        combinedArtworks.setArtworksSelected(delegateIndex, !isselected)
                                    }
                                }
                            }
                        }
                    }
                }

                Item {
                    height: 10
                }

                StyledText {
                    text: qsTr("Description:")
                    anchors.left: parent.left
                }

                Rectangle {
                    id: rect
                    width: parent.width
                    height: 25
                    color: Colors.defaultInputBackground
                    border.color: Colors.artworkActiveColor
                    border.width: descriptionTextInput.activeFocus ? 1 : 0
                    anchors.left: parent.left

                    StyledTextInput {
                        id: descriptionTextInput
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: 5
                        anchors.rightMargin: 5
                        anchors.verticalCenter: parent.verticalCenter
                        maximumLength: 250
                        text: combinedArtworks.description
                        onTextChanged: combinedArtworks.description = text
                        KeyNavigation.tab: titleTextInput
                    }
                }

                Item {
                    height: 8
                }

                RowLayout {
                    width: parent.width
                    spacing: 20

                    ColumnLayout {
                        spacing: 3
                        width: 325

                        RowLayout {
                            spacing: 5

                            StyledText {
                                text: qsTr("Title:")
                            }

                            StyledText {
                                text: qsTr("(same as Description if empty)")
                                color: Colors.defaultInputBackground
                            }
                        }

                        Rectangle {
                            id: anotherRect
                            anchors.left: parent.left
                            anchors.right: parent.right
                            height: 25
                            color: Colors.defaultInputBackground
                            border.color: Colors.artworkActiveColor
                            border.width: titleTextInput.activeFocus ? 1 : 0

                            StyledTextInput {
                                id: titleTextInput
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.leftMargin: 5
                                anchors.rightMargin: 5
                                anchors.verticalCenter: parent.verticalCenter
                                maximumLength: 200
                                focus: true
                                text: combinedArtworks.title
                                onTextChanged: combinedArtworks.title = text
                                KeyNavigation.tab: authorTextInput
                                KeyNavigation.backtab: descriptionTextInput
                            }
                        }
                    }

                    ColumnLayout {
                        spacing: 3
                        width: 325

                        StyledText {
                            text: qsTr("Author:")
                            anchors.left: parent.left
                        }

                        Rectangle {
                            id: yetAnotherRect
                            anchors.left: parent.left
                            anchors.right: parent.right
                            height: 25
                            color: Colors.defaultInputBackground
                            border.color: Colors.artworkActiveColor
                            border.width: authorTextInput.activeFocus ? 1 : 0

                            StyledTextInput {
                                id: authorTextInput
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.leftMargin: 5
                                anchors.rightMargin: 5
                                anchors.verticalCenter: parent.verticalCenter
                                maximumLength: 200
                                focus: true
                                text: combinedArtworks.author
                                onTextChanged: combinedArtworks.author = text
                                KeyNavigation.backtab: titleTextInput
                                Keys.onTabPressed: {
                                    flv.activateEdit()
                                }
                            }
                        }
                    }
                }

                Item {
                    height: 8
                }

                RowLayout {
                    anchors.left: parent.left
                    spacing: 5

                    StyledText {
                        id: keywordsLabel
                        text: qsTr("Keywords:")
                    }

                    StyledText {
                        text: qsTr("(comma-separated)")
                        color: Colors.defaultInputBackground
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledText {
                        text: combinedArtworks.keywordsCount
                        color: Colors.defaultInputBackground
                    }
                }

                Rectangle {
                    id: keywordsWrapper
                    border.color: Colors.artworkActiveColor
                    border.width: flv.isFocused ? 1 : 0
                    height: 100
                    anchors.rightMargin: 20
                    Layout.fillWidth: true
                    color: Colors.defaultInputBackground

                    function removeKeyword(index) {
                        combinedArtworks.removeKeywordAt(index)
                    }

                    function removeLastKeyword() {
                        combinedArtworks.removeLastKeyword()
                    }

                    function appendKeyword(keyword) {
                        combinedArtworks.appendKeyword(keyword)
                    }

                    function pasteKeywords(keywordsList) {
                        combinedArtworks.pasteKeywords(keywordsList)
                    }

                    MouseArea {
                        anchors.fill: parent
                        propagateComposedEvents: true
                        onClicked: {
                            flv.activateEdit()
                            mouse.accepted = false
                        }
                    }

                    StyledScrollView {
                        id: scroller
                        width: parent.width + 15
                        height: parent.height
                        highlightOnFocus: true

                        EditableTags {
                            id: flv
                            anchors.margins: { left: 5; top: 5; right: 0; bottom: 5 }
                            model: combinedArtworks.getKeywordsModel()

                            delegate: Rectangle {
                                id: itemWrapper
                                property int delegateIndex: index
                                property string keyword: modelData
                                color: Colors.defaultLightColor

                                width: childrenRect.width
                                height: childrenRect.height

                                RowLayout {
                                    spacing: 1

                                    Rectangle {
                                        id: tagTextRect
                                        width: childrenRect.width + 5
                                        height: 20
                                        color: "transparent"

                                        StyledText {
                                            anchors.left: parent.left
                                            anchors.leftMargin: 5
                                            anchors.top: parent.top
                                            anchors.bottom: parent.bottom
                                            verticalAlignment: Text.AlignVCenter
                                            text: modelData
                                            color: Colors.defaultControlColor
                                        }
                                    }

                                    CloseIcon {
                                        width: 14
                                        height: 14
                                        isActive: true
                                        anchors.verticalCenter: tagTextRect.verticalCenter
                                        onItemClicked: keywordsWrapper.removeKeyword(itemWrapper.delegateIndex)
                                    }

                                    Item {
                                        width: 1
                                    }
                                }
                            }

                            onTagAdded: {
                                keywordsWrapper.appendKeyword(text)
                            }

                            onRemoveLast: {
                                keywordsWrapper.removeLastKeyword()
                            }

                            onTagsPasted: {
                                keywordsWrapper.pasteKeywords(tagsList)
                            }
                        }
                    }
                }

                Item {
                    height: 15
                }

                Rectangle {
                    width: parent.width
                    color: "transparent"
                    height: 24

                    RowLayout {
                        anchors.fill: parent
                        spacing: 10

                        StyledButton {
                            width: 130
                            text: qsTr("Suggest keywords")
                            onClicked: {
                                var callbackObject = {
                                    promoteKeywords: function(keywords) {
                                        combinedArtworks.pasteKeywords(keywords)
                                    }
                                }

                                Common.launchDialog("Dialogs/KeywordsSuggestion.qml",
                                                       applicationWindow,
                                                       {callbackObject: callbackObject});
                            }
                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        StyledButton {
                            width: 130
                            text: qsTr("Save and append")
                            onClicked: {
                                combinedArtworks.saveAddKeywords()
                                closePopup()
                            }

                            tooltip: "Sets description and appends keywords in edited files"
                        }

                        StyledButton {
                            text: qsTr("Save and replace")
                            width: 130
                            onClicked: {
                                combinedArtworks.saveSetKeywords()
                                closePopup()
                            }

                            tooltip: "Sets description and replaces keywords in edited files"
                        }

                        StyledButton {
                            text: qsTr("Cancel")
                            width: 100
                            onClicked: {
                                closePopup()
                            }

                            tooltip: "Exits with no changes"
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: descriptionTextInput.forceActiveFocus()
}