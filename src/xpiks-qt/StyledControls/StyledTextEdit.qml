/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2016 Taras Kushnir <kushnirTV@gmail.com>
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
import "../Constants"
import "../Constants/UIConfig.js" as UIConfig

TextEdit {
    signal actionRightClicked();
    property string rightClickedWord;
    property bool isActive: true
    font.family: Qt.platform.os === "windows" ? "Arial" : "Helvetica"
    font.pixelSize: UIConfig.fontPixelSize * settingsModel.keywordSizeScale
    verticalAlignment: TextInput.AlignVCenter
    selectedTextColor: Colors.inputForegroundColor
    selectionColor: Colors.defaultControlColor
    renderType: Text.NativeRendering
    //selectByMouse: true
    cursorVisible: false
    wrapMode: TextEdit.NoWrap
    activeFocusOnPress: true
    color: (enabled && isActive) ? Colors.inputForegroundColor : Colors.inputInactiveForeground

    MouseArea{

        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            var oldVal = parent.cursorPosition;
            parent.cursorPosition = parent.positionAt(this.mouseX, this.mouseY);

            if (mouse.button == Qt.RightButton) {
                parent.selectWord();
                parent.rightClickedWord = parent.selectedText;
                console.log(parent.rightClickedWord);

                // get back to initial state
                parent.cursorPosition = oldVal;
                parent.deselect();
                actionRightClicked();
            }else {
                parent.forceActiveFocus();
            }
        }
        onDoubleClicked: {
            if (mouse.button == Qt.LeftButton) {
                parent.cursorPosition = parent.positionAt(this.mouseX, this.mouseY);
                parent.selectWord();
            }
        }
    }
}

