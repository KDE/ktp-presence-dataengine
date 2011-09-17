/*
 * Copyright (C) 2009-2011 Collabora Ltd <http://www.collabora.co.uk>
 * Copyright (C) 2011 Dario Freddi <drf@kde.org>
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

#ifndef PLASMA_DATAENGINE_PRESENCE_SETNICKNAMEJOB_H
#define PLASMA_DATAENGINE_PRESENCE_SETNICKNAMEJOB_H

#include <Plasma/ServiceJob>

#include <TelepathyQt4/Types>

namespace Tp {
    class PendingOperation;
}

class PresenceSource;

class SetNicknameJob : public Plasma::ServiceJob
{
    Q_OBJECT

public:
    SetNicknameJob(PresenceSource *source,
                const QMap< QString, QVariant >& parameters,
                QObject* parent = 0);
    virtual void start();

private Q_SLOTS:
    void onSetNicknameFinished(Tp::PendingOperation *op);

private:
    Tp::AccountPtr m_account;
};

#endif
