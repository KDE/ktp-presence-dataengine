/*
 * Copyright (C) 2009 Collabora Ltd <http://www.collabora.co.uk>
 * Copyright (C) 2009 Andre Moreira Magalhaes <andrunko@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License version 2 as
 * published by the Free Software Foundation
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

#include "presenceservice.h"

#include "setavatarjob.h"
#include "setnicknamejob.h"
#include "setrequestedpresencejob.h"

PresenceService::PresenceService(PresenceSource *parent)
    : Plasma::Service(parent),
      m_source(parent)
{
    // Set the name so the kcfg file can be found.
    setName("presence");
}

PresenceService::~PresenceService()
{
}

Plasma::ServiceJob *PresenceService::createJob(const QString &operation,
        QMap<QString, QVariant> &parameters)
{
    if (operation == "setPresence") {
        if (parameters.contains("type_id") &&
            parameters.contains("status_message")) {
            return new SetRequestedPresenceJob(m_source, parameters);
        }
    } else if (operation == "setNickname") {
        if (parameters.contains("nickname")) {
            return new SetNicknameJob(m_source, parameters);
        }
    } else if (operation == "setAvatar") {
        if (parameters.contains("url") || parameters.contains("rawdata")) {
            return new SetAvatarJob(m_source, parameters);
        }
    }

    // If we don't know what to do for this operation, default to failing.
    return new Plasma::ServiceJob(m_source->objectName(), operation,
            parameters, this);
}

#include "presenceservice.moc"

