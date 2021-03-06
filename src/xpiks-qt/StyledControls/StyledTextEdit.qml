/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
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
    id: textEditHost
    signal actionRightClicked();
    property string rightClickedWord
    property bool isActive: true
    property bool userDictEnabled: false
    font.family: Qt.platform.os === "windows" ? "Arial" : "Helvetica"
    font.pixelSize: UIConfig.fontPixelSize * settingsModel.keywordSizeScale
    verticalAlignment: TextInput.AlignVCenter
    selectedTextColor: Colors.inputForegroundColor
    selectionColor: Colors.inputInactiveBackground
    renderType: Text.NativeRendering
    selectByMouse: true
    cursorVisible: false
    wrapMode: TextEdit.NoWrap
    activeFocusOnPress: true
    color: (enabled && isActive) ? Colors.inputForegroundColor : Colors.inputInactiveForeground

    function isSeparator(position) {
        var separators = " ,.:;\\|<>()-";
        var symbol = text[position];
        return (separators.indexOf(symbol) >= 0);
    }

    function isRightBound(position, maxPos) {
        if (position >= maxPos) {
            return true;
        }

        return !isSeparator(position) && isSeparator(position + 1);
    }

    function getRightBound(position) {
        var cur = position;
        var maxPos = text.length

        while (!isRightBound(cur, maxPos - 1)) {
            cur++;
        }

        return cur + 1;
    }

    function isLeftBound(position) {
        if (position === 0) {
            return true;
        }

        return !isSeparator(position) && isSeparator(position - 1);
    }

    function getLeftBound(position) {
        var cur = position;
        while (!isLeftBound(cur)) {
            cur--;
        }

        return cur;
    }

    function getWordByPosition(textPosition) {
        var leftBound = getLeftBound(textPosition);
        var rightBound = getRightBound(textPosition);
        var word = getText(leftBound, rightBound);
        return word
    }

    MouseArea {
        enabled: textEditHost.enabled && textEditHost.userDictEnabled
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        propagateComposedEvents: true

        onClicked: {
            if (mouse.button == Qt.RightButton) {
                var textPosition = positionAt(mouse.x, mouse.y);
                var word = textEditHost.getWordByPosition(textPosition);

                console.log("Detected word under click: " + word);
                parent.rightClickedWord = word;

                if (word.length !== 0) {
                    actionRightClicked();
                }
            }
        }        
    }
}

