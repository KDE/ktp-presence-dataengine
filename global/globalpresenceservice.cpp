/*
 * Copyright (C) 2011 Francesco Nwokeka <francesco.nwokeka@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "globalpresenceservice.h"
#include "setglobalpresencejob.h"

GlobalPresenceService::GlobalPresenceService(GlobalPresenceSource *parent)
    : Service(parent)
    , m_source(parent)
{
    setName("presence");    // can i do this?
}

GlobalPresenceService::~GlobalPresenceService()
{
}

Plasma::ServiceJob* GlobalPresenceService::createJob(const QString& operation, QMap<QString, QVariant> &parameters)
{
    if (operation == "setGlobalPresence") {
        if (parameters.contains("presence")) {
            return new SetGlobalPresenceJob(m_source, parameters);
        }
    }

    // If we don't know what to do for this operation, default to failing.
    return new Plasma::ServiceJob(m_source->objectName(), operation,
                                  parameters, this);
}

