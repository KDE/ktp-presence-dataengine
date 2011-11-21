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

#include "../common/global-presence.h"
#include "setglobalpresencejob.h"

SetGlobalPresenceJob::SetGlobalPresenceJob(GlobalPresenceSource *source, const QMap<QString, QVariant> &parameters, QObject *parent)
    : Plasma::ServiceJob(source->objectName(), "setGlobalPresence", parameters, parent)
    , m_globalPresence(source->globalPresence())
{
}

void SetGlobalPresenceJob::start()
{
    QString presenceType = parameters()["presence"].toString();
    QString presenceMessage = parameters()["presenceMessage"].toString();

    presenceType.toLower();

    if (presenceType == "online" || presenceType == "available") {
        m_globalPresence->setPresence(Tp::Presence::available(presenceMessage));
    } else if (presenceType == "busy") {
        m_globalPresence->setPresence(Tp::Presence::busy(presenceMessage));
    } else if (presenceType == "away") {
        m_globalPresence->setPresence(Tp::Presence::away(presenceMessage));
    } else if (presenceType == "away-extended") {
        m_globalPresence->setPresence(Tp::Presence::xa(presenceMessage));
    } else if (presenceType == "invisible" || presenceType == "hidden") {
        m_globalPresence->setPresence(Tp::Presence::hidden());
    } else if (presenceType == "offline") {
        m_globalPresence->setPresence(Tp::Presence::offline());
    } else {
        m_globalPresence->setPresence(Tp::Presence::offline());
    }
}
