/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014 Taras Kushnir <kushnirTV@gmail.com>
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

import QtQuick 2.4
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import "../Constants"
import "../Constants/Colors.js" as Colors;
import "../Components"
import "../StyledControls"

ApplicationWindow {
    id: settingsWindow
    modality: "ApplicationModal"
    width: 640
    height: 265
    minimumWidth: width
    maximumWidth: width
    minimumHeight: height
    maximumHeight: height
    flags: Qt.Tool

    function closeSettings() {
        settingsWindow.destroy();
    }

    property string defaultExifTool: "exiftool"
    property string defaultCurl: "curl"
    property double defaultMaxKeywords: 50
    property int defaultMaxDescription: 200
    property double defaultMinMegapixels: 4.0

    property string exiftoolpathkey: appSettings.exifToolPathKey
    property string exifToolPath: appSettings.value(exiftoolpathkey, defaultExifTool)

    property string curlpathkey: appSettings.curlPathKey
    property string curlPath: appSettings.value(curlpathkey, defaultCurl)

    property string maxkeywordscountkey: appSettings.maxKeywordsCount;
    property int maxKeywordsCount: appSettings.value(maxkeywordscountkey, defaultMaxKeywords)

    property string maxdescriptionlengthkey: appSettings.maxDescriptionLength;
    property int maxDescriptionLength: appSettings.value(maxdescriptionlengthkey, defaultMaxDescription)

    property string minmegapixelskey: appSettings.minMegapixelCount;
    property double minMegapixelCount: appSettings.value(minmegapixelskey, defaultMinMegapixels)

    FileDialog {
        id: exifToolFileDialog
        title: "Please choose ExifTool location"
        selectExisting: true
        selectMultiple: false
        nameFilters: [ "All files (*)" ]

        onAccepted: {
            console.log("You chose: " + exifToolFileDialog.fileUrl)
            var path = exifToolFileDialog.fileUrl.toString().replace(/^(file:\/{3})/,"");
            exifToolPath = decodeURIComponent(path);
            exifToolText.text = exifToolPath;
        }

        onRejected: {
            console.log("File dialog canceled")
        }
    }

    FileDialog {
        id: curlFileDialog
        title: "Please choose curl location"
        selectExisting: true
        selectMultiple: false
        nameFilters: [ "All files (*)" ]

        onAccepted: {
            console.log("You chose: " + curlFileDialog.fileUrl)
            var path = curlFileDialog.fileUrl.toString().replace(/^(file:\/{3})/,"");
            curlPath = decodeURIComponent(path);
            curlText.text = curlPath;
        }

        onRejected: {
            console.log("File dialog canceled")
        }
    }

    Rectangle {
        color: Colors.selectedArtworkColor
        anchors.fill: parent

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 20

            GridLayout {
                width: parent.width
                Layout.fillHeight: true
                rows: 5
                columns: 4
                rowSpacing: 15
                columnSpacing: 5

                StyledText {
                    Layout.row: 0
                    Layout.column: 0
                    Layout.fillWidth: true
                    Layout.maximumWidth: 130

                    horizontalAlignment: Text.AlignRight
                    text: qsTr("ExifTool path:")
                }

                Rectangle {
                    Layout.row: 0
                    Layout.column: 1
                    height: childrenRect.height
                    width: childrenRect.width + 5
                    color: Colors.defaultInputBackground
                    border.color: Colors.artworkActiveColor
                    border.width: exifToolText.activeFocus ? 1 : 0

                    StyledTextInput {
                        id: exifToolText
                        width: 300
                        height: 24
                        clip: true
                        text: exifToolPath
                        anchors.left: parent.left
                        anchors.leftMargin: 5
                        KeyNavigation.tab: curlText
                    }
                }

                StyledButton {
                    Layout.row: 0
                    Layout.column: 2
                    Layout.fillWidth: true
                    text: qsTr("Select...")
                    width: 50
                    Layout.preferredWidth: 50
                    onClicked: exifToolFileDialog.open()
                }

                StyledButton {
                    Layout.row: 0
                    Layout.column: 3                    
                    Layout.fillWidth: true
                    text: qsTr("Reset")
                    width: 50
                    Layout.preferredWidth: 50
                    onClicked: exifToolText.text = defaultExifTool
                }

                StyledText {
                    Layout.row: 1
                    Layout.column: 0
                    Layout.fillWidth: true
                    Layout.maximumWidth: 130
                    horizontalAlignment: Text.AlignRight
                    text: qsTr("Curl path:")
                }

                Rectangle {
                    Layout.row: 1
                    Layout.column: 1
                    height: childrenRect.height
                    width: childrenRect.width + 5
                    color: Colors.defaultInputBackground
                    border.color: Colors.artworkActiveColor
                    border.width: curlText.activeFocus ? 1 : 0

                    StyledTextInput {
                        id: curlText
                        width: 300
                        height: 24
                        clip: true
                        text: curlPath
                        anchors.left: parent.left
                        anchors.leftMargin: 5
                        KeyNavigation.backtab: exifToolText
                        KeyNavigation.tab: megapixelsCount
                    }
                }

                StyledButton {
                    Layout.row: 1
                    Layout.column: 2
                    Layout.fillWidth: true
                    text: qsTr("Select...")
                    width: 50
                    Layout.preferredWidth: 50
                    onClicked: curlFileDialog.open()
                }

                StyledButton {
                    Layout.row: 1
                    Layout.column: 3
                    Layout.fillWidth: true
                    text: qsTr("Reset")
                    width: 50
                    Layout.preferredWidth: 50
                    onClicked: curlText.text = defaultCurl
                }

                StyledText {
                    Layout.row: 2
                    Layout.column: 0
                    Layout.fillWidth: true
                    Layout.maximumWidth: 130
                    horizontalAlignment: Text.AlignRight
                    text: qsTr("Minimum megapixels:")
                }

                Rectangle {
                    Layout.row: 2
                    Layout.column: 1
                    height: childrenRect.height
                    width: childrenRect.width + 5
                    color: Colors.defaultInputBackground
                    border.color: Colors.artworkActiveColor
                    border.width: megapixelsCount.activeFocus ? 1 : 0

                    StyledTextInput {
                        id: megapixelsCount
                        width: 100
                        height: 24
                        clip: true
                        text: minMegapixelCount
                        anchors.left: parent.left
                        anchors.leftMargin: 5
                        KeyNavigation.backtab: curlText
                        KeyNavigation.tab: keywordsCount

                        validator: DoubleValidator {
                            bottom: 0
                            top: 100
                            decimals: 1
                            notation: "StandardNotation"
                        }
                    }
                }

                StyledText {
                    Layout.row: 3
                    Layout.column: 0
                    Layout.fillWidth: true
                    Layout.maximumWidth: 130
                    horizontalAlignment: Text.AlignRight
                    text: qsTr("Max keywords count:")
                }

                Rectangle {
                    Layout.row: 3
                    Layout.column: 1
                    height: childrenRect.height
                    width: childrenRect.width + 5
                    color: Colors.defaultInputBackground
                    border.color: Colors.artworkActiveColor
                    border.width: keywordsCount.activeFocus ? 1 : 0

                    StyledTextInput {
                        id: keywordsCount
                        width: 100
                        height: 24
                        clip: true
                        text: maxKeywordsCount
                        anchors.left: parent.left
                        anchors.leftMargin: 5
                        KeyNavigation.backtab: megapixelsCount
                        KeyNavigation.tab: descriptionLength

                        validator: IntValidator {
                            bottom: 0
                            top: 200
                        }
                    }
                }

                StyledText {
                    Layout.row: 4
                    Layout.column: 0
                    Layout.fillWidth: true
                    Layout.maximumWidth: 130
                    horizontalAlignment: Text.AlignRight
                    text: qsTr("Max description length:")
                }

                Rectangle {
                    Layout.row: 4
                    Layout.column: 1
                    height: childrenRect.height
                    width: childrenRect.width + 5
                    color: Colors.defaultInputBackground
                    border.color: Colors.artworkActiveColor
                    border.width: descriptionLength.activeFocus ? 1 : 0

                    StyledTextInput {
                        id: descriptionLength
                        width: 100
                        height: 24
                        clip: true
                        text: maxDescriptionLength
                        anchors.left: parent.left
                        anchors.leftMargin: 5
                        KeyNavigation.backtab: keywordsCount

                        validator: IntValidator {
                            bottom: 0
                            top: 1000
                        }
                    }
                }
            }

            RowLayout {
                height: 24
                spacing: 10

                Item {
                    Layout.fillWidth: true
                }

                StyledButton {
                    text: qsTr("Save and Exit")
                    width: 150
                    onClicked: {
                        exifToolPath = exifToolText.text
                        appSettings.setValue(exiftoolpathkey, exifToolPath)

                        curlPath = curlText.text
                        appSettings.setValue(curlpathkey, curlPath)

                        minMegapixelCount = parseFloat(megapixelsCount.text)
                        appSettings.setValue(minmegapixelskey, minMegapixelCount)

                        maxKeywordsCount = parseInt(keywordsCount.text)
                        appSettings.setValue(maxkeywordscountkey, maxKeywordsCount)

                        maxDescriptionLength = parseInt(descriptionLength.text)
                        appSettings.setValue(maxdescriptionlengthkey, maxDescriptionLength)

                        closeSettings()
                    }
                }

                StyledButton {
                    text: qsTr("Exit")
                    width: 100
                    onClicked: closeSettings()
                }
            }
        }
    }

    Component.onCompleted: exifToolText.forceActiveFocus()
}

