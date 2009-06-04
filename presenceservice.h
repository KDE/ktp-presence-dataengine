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

#ifndef PLASMA_DATAENGINE_PRESENCE_PRESENCESERVICE_H
#define PLASMA_DATAENGINE_PRESENCE_PRESENCESERVICE_H

#include "presencesource.h"

#include <Plasma/Service>

class PresenceService : public Plasma::Service
{
    Q_OBJECT

public:
    PresenceService(PresenceSource *parent);
    ~PresenceService();

protected:
    Plasma::ServiceJob *createJob(const QString &operation,
            QMap<QString, QVariant> &parameters);

private:
    PresenceSource *m_source;
};

#endif

