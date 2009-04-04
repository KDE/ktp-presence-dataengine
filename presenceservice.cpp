/*
 *   Copyright (C) 2009 Collabora Ltd <http://www.collabora.co.uk>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License version 2 as
 *   published by the Free Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "presenceservice.h"

#include "setrequestedpresencejob.h"

PresenceService::PresenceService(PresenceSource *parent)
 : Plasma::Service(parent),
   m_source(parent)
{
    kDebug();   // Output the method we are in.

    // Set the name so the kcfg file can be found.
    setName("presence");
}

PresenceService::~PresenceService()
{
    kDebug();   // Output the method we are in.
}


Plasma::ServiceJob * PresenceService::createJob(const QString& operation, QMap<QString, QVariant>& parameters)
{
    if (operation == "setRequestedPresence") {
        return new SetRequestedPresenceJob(m_source, parameters);
    }

    // default case is to fail:
    return new Plasma::ServiceJob(m_source->objectName(), operation, parameters, this);
}


#include "presenceservice.moc"

