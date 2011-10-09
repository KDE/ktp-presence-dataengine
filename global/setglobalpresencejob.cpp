/*
 * Copyright (C) 20011 Francesco Nwokeka <francesco.nwokeka@gmail.com>
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

#include "global-presence.h"
#include "setglobalpresencejob.h"

SetGlobalPresenceJob::SetGlobalPresenceJob(GlobalPresenceSource *source, const QMap<QString, QVariant> &parameters, QObject *parent)
    : Plasma::ServiceJob(source->objectName(), "setGlobalPresence", parameters, parent)
    , m_globalPresence(source->globalPresence())
{
}

void SetGlobalPresenceJob::start()
{
    QString newPresence = parameters()["presence"].toString();
    newPresence.toLower();

    if (newPresence == "online" || newPresence == "available") {
        m_globalPresence->setPresence(Tp::Presence::available());
    } else if (newPresence == "busy") {
        m_globalPresence->setPresence(Tp::Presence::busy());
    } else if (newPresence == "away") {
        m_globalPresence->setPresence(Tp::Presence::away());
    } else if (newPresence == "invisible" || newPresence == "hidden") {
        m_globalPresence->setPresence(Tp::Presence::hidden());
    } else if (newPresence == "offline") {
        m_globalPresence->setPresence(Tp::Presence::offline());
    } else {
        m_globalPresence->setPresence(Tp::Presence::offline());
    }
}
