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
import "../Components"
import "../Constants"
import "../Constants/Colors.js" as Colors

TabView {
    property double tabBarWidth: 1
    property double tabsHeight: 24
    property color backgroundColor: Colors.selectedArtworkColor

    style: TabViewStyle {
        frameOverlap: -tabBarWidth
        tabOverlap: -2

        frame: Rectangle {
            color: backgroundColor
        }

        leftCorner: Rectangle {
            implicitWidth: 5
            implicitHeight: tabBarWidth
            color: "transparent"
        }

        tabBar : Rectangle {
            color: backgroundColor

            CustomBorder {
                commonBorder: false
                //color: Colors.artworkActiveColor
                borderColor: Colors.artworkActiveColor
                lBorderwidth: 0
                rBorderwidth: 0
                tBorderwidth: 0
                bBorderwidth: tabBarWidth
            }
        }

        tab: Rectangle {
            color: styleData.selected ? backgroundColor : Colors.itemsSourceSelected
            implicitWidth: Math.max(text.width + 10, 80)
            implicitHeight: tabsHeight

            StyledText {
                id: text
                anchors.centerIn: parent
                text: styleData.title
                color: styleData.hovered ? Colors.artworkActiveColor : (styleData.selected ? Colors.defaultLightColor : Colors.defaultInputBackground)
            }

            CustomBorder {
                commonBorder: false
                //color: Colors.artworkActiveColor
                borderColor: backgroundColor
                lBorderwidth: 0
                rBorderwidth: 0
                tBorderwidth: 0
                bBorderwidth: styleData.selected ? tabBarWidth : 0
            }

            CustomBorder {
                commonBorder: false
                //color: Colors.artworkActiveColor
                borderColor: Colors.artworkActiveColor
                lBorderwidth: styleData.selected ? tabBarWidth : 0
                rBorderwidth: styleData.selected ? tabBarWidth : 0
                tBorderwidth: styleData.selected ? tabBarWidth : 0
                bBorderwidth: 0
            }
        }
    }
}