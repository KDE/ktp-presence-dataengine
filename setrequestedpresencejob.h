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

#ifndef PLASMA_DATAENGINE_PRESENCE_SETREQUESTEDPRESENCEJOB_H
#define PLASMA_DATAENGINE_PRESENCE_SETREQUESTEDPRESENCEJOB_H

#include <Plasma/ServiceJob>

#include <TelepathyQt4/Types>

namespace Tp {
    class PendingOperation;
}

class PresenceSource;

class SetRequestedPresenceJob : public Plasma::ServiceJob
{
    Q_OBJECT

public:
    SetRequestedPresenceJob(PresenceSource *source,
            const QMap<QString, QVariant> &parameters,
            QObject *parent = 0);
    void start();

private Q_SLOTS:
    void onSetRequestedPresenceFinished(Tp::PendingOperation *op);

private:
    Tp::SimplePresence parametersToSimplePresence(const QVariantMap &parameters);

    Tp::AccountPtr m_account;
};

#endif

