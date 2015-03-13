/**
 ******************************************************************************
 *
 * @file       updaterplugin.cpp
 * @author     Tau Labs, http://taulabs.org, Copyright (C) 2013-2014
 * @addtogroup GCSPlugins GCS Plugins
 * @{
 * @addtogroup Updater Plugin
 * @{
 * @brief Updater Plugin
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 3 of the License, or 
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License 
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along 
 * with this program; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "updaterplugin.h"

#include <QtPlugin> 
#include <QStringList> 
#include <QDebug>

#include "extensionsystem/pluginmanager.h"

// for XML object
#include <QDomDocument>


UpdaterPlugin::UpdaterPlugin()
{ 
   // Do nothing 
} 

UpdaterPlugin::~UpdaterPlugin()
{ 
   // Do nothing 
} 

bool UpdaterPlugin::initialize(const QStringList& args, QString *errMsg)
{ 
    Q_UNUSED(args);
    Q_UNUSED(errMsg);
    mf = new UpdaterFactory(this);
    addAutoReleasedObject(mf);
    return true;
}

void UpdaterPlugin::shutdown()
{ 
   // Do nothing 
}
void UpdaterPlugin::extensionsInitialized()
{
}
