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

#ifndef GLOBAL_PRESENCE_SOURCE_H
#define GLOBAL_PRESENCE_SOURCE_H

#include <Plasma/DataContainer>

#include <TelepathyQt4/AccountManager>

class GlobalPresence;

class GlobalPresenceSource : public Plasma::DataContainer
{
    Q_OBJECT

public:
    GlobalPresenceSource(QObject *parent = 0);
    ~GlobalPresenceSource();

    Plasma::Service *createService();
    GlobalPresence *globalPresence() const;
    void setGlobalPresenceAccountManager(const Tp::AccountManagerPtr &accountMgr);

private slots:
    void onCurrentPresenceChanged(Tp::Presence newPresence);

private:
    GlobalPresence *m_globalPresence;
};



#endif  // GLOBAL_PRESENCE_SOURCE_H
