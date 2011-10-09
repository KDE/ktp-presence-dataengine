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

#ifndef PLASMA_DATAENGINE_PRESENCE_SETGLOBALPRESENCEJOB_H
#define PLASMA_DATAENGINE_PRESENCE_SETGLOBALPRESENCEJOB_H

#include "globalpresencesource.h"

#include <Plasma/ServiceJob>

#include <TelepathyQt4/Types>

class GlobalPresence;

class SetGlobalPresenceJob : public Plasma::ServiceJob
{
    Q_OBJECT
public:
    SetGlobalPresenceJob(GlobalPresenceSource *source, const QMap<QString, QVariant> &parameters, QObject *parent = 0);
    void start();

private:
    GlobalPresence *m_globalPresence;
};

#endif //   PLASMA_DATAENGINE_PRESENCE_SETGLOBALPRESENCEJOB_H