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

#include "pluginwrapper.h"
#include <QDebug>
#include "xpiksplugininterface.h"
#include "ipluginaction.h"
#include "pluginactionsmodel.h"

namespace Plugins {
    PluginWrapper::PluginWrapper(XpiksPluginInterface *pluginInterface, int pluginID):
        m_PluginInterface(pluginInterface),
        m_ActionsModel(pluginInterface->getExportedActions(), pluginID),
        m_PluginID(pluginID),
        m_IsEnabled(true),
        m_PrettyName(pluginInterface->getPrettyName()),
        m_VersionString(pluginInterface->getVersionString()),
        m_Author(pluginInterface->getAuthor())
    {
    }

    void PluginWrapper::enablePlugin() {
        qInfo() << "PluginWrapper::enablePlugin #" << getPrettyName();

        try {
            m_PluginInterface->enablePlugin();
            m_IsEnabled = true;
        }
        catch(...) {
            qWarning() << "PluginWrapper::enablePlugin #" << "Exception while enabling plugin";
        }
    }

    void PluginWrapper::disablePlugin() {
        qInfo() << "PluginWrapper::disablePlugin #" << getPrettyName();

        try {
            // set disabled in any case
            m_IsEnabled = false;
            m_PluginInterface->disablePlugin();
        }
        catch(...) {
            qWarning() << "PluginWrapper::disablePlugin #" << "Exception while disabling plugin";
        }
    }

    void PluginWrapper::triggerActionSafe(int actionID) const {
        qInfo() << "PluginWrapper::triggerAction #" << getPrettyName() << "executing action:" << actionID;

        try {
            if (m_IsEnabled) {
                m_PluginInterface->executeAction(actionID);
            } else {
                qWarning() << "PluginWrapper::triggerAction #" << getPrettyName() << "is disabled";
            }
        }
        catch (...) {
            qWarning() << "PluginWrapper::triggerAction #" << "Exception while triggering action for plugin ID" << m_PluginID;
        }
    }

    void PluginWrapper::finalizePlugin() {
        qInfo() << "PluginWrapper::finalizePlugin #" << getPrettyName();

        try {
            m_PluginInterface->finalizePlugin();
        }
        catch (...) {
            qWarning() << "PluginWrapper::finalizePlugin #" << "Exception on finalization";
        }
    }
}
