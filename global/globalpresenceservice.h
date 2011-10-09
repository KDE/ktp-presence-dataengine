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

#ifndef PLASMA_DATAENGINE_PRESENCE_GLOBALPRESENCESERVICE_H
#define PLASMA_DATAENGINE_PRESENCE_GLOBALPRESENCESERVICE_H

#include "globalpresencesource.h"

#include <Plasma/Service>

class GlobalPresenceService : public Plasma::Service
{
    Q_OBJECT

public:
    GlobalPresenceService(GlobalPresenceSource *parent);
    ~GlobalPresenceService();

protected:
    Plasma::ServiceJob *createJob(const QString &operation, QMap<QString, QVariant> &parameters);

private:
    GlobalPresenceSource *m_source;
};


#endif  // PLASMA_DATAENGINE_PRESENCE_GLOBALPRESENCESERVICE_H