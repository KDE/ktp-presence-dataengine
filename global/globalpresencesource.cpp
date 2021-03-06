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
#include "globalpresencesource.h"

#include <KTelepathy/global-presence.h>

GlobalPresenceSource::GlobalPresenceSource(QObject* parent)
    : DataContainer(parent)
    , m_globalPresence(new KTp::GlobalPresence(parent))
{
    // name of the source
    setObjectName("GlobalPresence");

    // global presence source data
    setData("currentPresence", "");
    setData("requestedPresence", "");
    setData("presenceMessage", "");
}

GlobalPresenceSource::~GlobalPresenceSource()
{
}

Plasma::Service* GlobalPresenceSource::createService()
{
    return new GlobalPresenceService(this);
}

KTp::GlobalPresence *GlobalPresenceSource::globalPresence() const
{
    return m_globalPresence;
}

void GlobalPresenceSource::onCurrentPresenceChanged(Tp::Presence newPresence)
{
    switch (newPresence.type()) {
        case Tp::ConnectionPresenceTypeAvailable:
            setData("currentPresence", "online");
            break;
        case Tp::ConnectionPresenceTypeAway:
            setData("currentPresence", "away");
            break;
        case Tp::ConnectionPresenceTypeExtendedAway:
            setData("currentPresence", "away-extended");
            break;
        case Tp::ConnectionPresenceTypeBusy:
            setData("currentPresence", "busy");
            break;
        case Tp::ConnectionPresenceTypeHidden:
            setData("currentPresence", "invisible");
            break;
        case Tp::ConnectionPresenceTypeOffline:
            setData("currentPresence", "offline");
            break;
        default:
            setData("currentPresence", "offline");
            break;
    }

    setData("presenceMessage", newPresence.statusMessage());
    checkForUpdate();
}

void GlobalPresenceSource::setGlobalPresenceAccountManager(const Tp::AccountManagerPtr& accountMgr)
{
    if (!accountMgr || !accountMgr->isValid()) {
        kWarning() << "GlobalPresenceSource::setGlobalPresenceAccountManager Invalid account manager pointer";
        return;
    }

    m_globalPresence->setAccountManager(accountMgr);

    // setup connections and initialise with current data.
    connect(m_globalPresence, SIGNAL(currentPresenceChanged(Tp::Presence)), this, SLOT(onCurrentPresenceChanged(Tp::Presence)));
    onCurrentPresenceChanged(m_globalPresence->currentPresence());
}
